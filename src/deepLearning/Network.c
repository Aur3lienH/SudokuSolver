#include <stdlib.h>
#include "deepLearning/Network.h"
#include "matrix/BlockMatrix.h"
#include "deepLearning/layer/FCL.h"
#include "deepLearning/layer/InputLayer.h"
#include "tools/ProgressBar.h"
#include <pthread.h>
#include "deepLearning/Barrier.h"
#include "deepLearning/applications/Mnist.h"
#include "deepLearning/layer/Dropout.h"
#include "deepLearning/layer/ReshapeLayer.h"
#include "tools/ConsoleTools.h"
#include "deepLearning/layer/MaxPoolLayer.h"
#include "deepLearning/layer/ReshapeLayer.h"
#include "deepLearning/layer/ConvLayer.h"


Network* N_CreateNetwork()
{
    Network* network = (Network*)malloc(sizeof(Network));
    network->layersCount = 0;
    network->layers = NULL;
    network->accumulator = NULL;
    return network;
}

void N_Free(Network* n)
{
    M_Free(n->lossDerivative);
    for (size_t i = 0; i < n->layersCount; i++)
    {
        n->layers[i]->Free(n->layers[i]->layerPtr);
    }
    free(n->loss);
    free(n->layers);
    free(n);
}

void N_Compile_L2(Network* n,Loss* loss, float lambda)
{
    n->loss = loss;
    LayerShape* previousLayerShape = NULL;
    for (size_t i = 0; i < n->layersCount; i++)
    {
        n->layers[i]->layerPos = i;
        n->layers[i]->Compile(n->layers[i]->layerPtr,previousLayerShape);
        previousLayerShape = n->layers[i]->layerShape;
    }
    LayerShape* lastLayerShape = n->layers[n->layersCount - 1]->layerShape;
    n->lossDerivative = M_Create_3D(lastLayerShape->x,lastLayerShape->y,lastLayerShape->z);
    n->accumulator = (float*)malloc(sizeof(float));
    *n->accumulator = 0;
    n->lambda = lambda;
}

void N_Compile(Network* n, Loss* loss)
{
    N_Compile_L2(n,loss,0);
}

Matrix* N_Process(Network* n, Matrix* i)
{
    Matrix* input = i;
    for (size_t i = 0; i < n->layersCount; i++)
    {
        input = n->layers[i]->Process((n->layers[i])->layerPtr,input);
    }
    //Reset all the reshape layers
    N_Reset(n);
    return input;
}

float N_FeedForward(Network* n, Matrix* input, Matrix* desiredOutput)
{
    for (size_t i = 0; i < n->layersCount; i++)
    {
        input = n->layers[i]->FeedForward((n->layers[i])->layerPtr,input);
    }
    return n->loss->Compute(input,desiredOutput,n->accumulator,n->lambda);
}

void* N_BackpropagationThread(void* ptr)
{
    ThreadArgs* args = (ThreadArgs*)ptr;
    size_t batchCount = args->dataset->size / args->batchSize;
    for (size_t i = 0; i < args->epochsCount; i++)
    {
        for (size_t k = 0; k < batchCount; k++)
        {
            pthread_barrier_wait(args->barrier);
            pthread_mutex_lock(args->mutex);
            for (size_t j = 0; j < args->batchSize; j++)
            {
                N_Backpropagation(args->n,args->dataset->data[0][k*args->batchSize+j],args->dataset->data[1][k*args->batchSize+j]);
            }
            pthread_mutex_unlock(args->mutex);
            
        }
    }
    return NULL;
}

double N_Backpropagation(Network* n, Matrix* input, Matrix* desiredOutput)
{
    double loss = N_FeedForward(n,input,desiredOutput);
    n->loss->ComputeDerivative(n->layers[n->layersCount - 1]->outputs,desiredOutput,n->lossDerivative);
    N_BackPropagate(n,n->lossDerivative);
    return loss;
}

void N_Train(Network* n, Dataset* dataset,size_t testSetSize, size_t batchSize, size_t epochsCount, size_t threadsCount, float learningRate)
{
    if(dataset->size < testSetSize)
    {
        printf("The test set size is bigger than the dataset size\n");
        exit(-1);
    }

    size_t dividedDatasetSize = dataset->size / threadsCount;
    size_t auxBatchSize = batchSize / threadsCount;
    size_t batchCount = dividedDatasetSize / auxBatchSize;
    ProgressBar* progressBar = PB_Create(batchCount * epochsCount);
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * (threadsCount - 1));

    N_ShuffleDataset(dataset);
    Dataset* testSet = malloc(sizeof(Dataset));
    testSet->size = testSetSize;
    testSet->data = (Matrix***)malloc(sizeof(Matrix**) * 2);
    testSet->data[0] = dataset->data[0] + dataset->size - testSet->size;
    testSet->data[1] = dataset->data[1] + dataset->size - testSet->size;

    dataset->size -= testSet->size;

    Network** networks = (Network**)malloc(sizeof(Network*) * threadsCount);
    Dataset** datasets = (Dataset**)malloc(sizeof(Dataset*) * (threadsCount - 1));
    Matrix**** datasetParts = (Matrix****)malloc(sizeof(Matrix***) * (threadsCount - 1));
    pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t) * (threadsCount - 1));

    pthread_barrier_t barrier;
    #if CUSTOM_BARRIER
        pthread_barrier_init(&barrier, threadsCount);
    #else
        pthread_barrier_init(&barrier, NULL, threadsCount);
    #endif
    ThreadArgs* args = (ThreadArgs*)malloc(sizeof(ThreadArgs) * threadsCount);
    networks[0] = n;
    for (size_t i = 0; i < threadsCount-1; i++)
    {
        
        networks[i+1] = N_CreateNetwork();
        networks[i+1]->layersCount = n->layersCount;
        networks[i+1]->layers = (Layer**)malloc(sizeof(Layer*) * networks[i+1]->layersCount);
        for (size_t j = 0; j < networks[i+1]->layersCount; j++)
        {
            networks[i+1]->layers[j] = n->layers[j]->Copy(n->layers[j]->layerPtr);
        }
        N_Compile_L2(networks[i+1],n->loss,n->lambda);

        pthread_mutex_init(mutex + i,NULL);

        args[i].n = networks[i+1];
        datasetParts[i] = (Matrix***)malloc(sizeof(Matrix**) * 2);
        datasetParts[i][0] = dataset->data[0] + (dataset->size / threadsCount) * (i+1);
        datasetParts[i][1] = dataset->data[1] + (dataset->size / threadsCount) * (i+1);
        datasets[i] = Dataset_Create(datasetParts[i],dividedDatasetSize);
        args[i].dataset = datasets[i];
        args[i].batchSize = auxBatchSize;
        args[i].epochsCount = epochsCount;
        args[i].mutex = mutex + i;
        args[i].barrier = &barrier;


        pthread_create(threads + i,NULL,N_BackpropagationThread,args + i);
    }
    for (size_t e = 0; e < epochsCount; e++)
    {
        size_t i = 0;
        N_ShuffleDataset(dataset);
        for(size_t k = 0; k < batchCount; k++)
        {
            pthread_barrier_wait(&barrier);
            float globalLoss = 0;
            
            for (size_t j = 0; j < auxBatchSize; j++)
            {
                globalLoss += N_Backpropagation(n,dataset->data[0][i + j],dataset->data[1][i + j]);
            }
            
            for (size_t i = 0; i < threadsCount-1; i++)
            {
                pthread_mutex_lock(mutex + i);
            }
            
            PB_ChangeProgress(progressBar,e * batchCount + k,globalLoss / auxBatchSize);
            for (size_t i = 1; i < threadsCount; i++)
            {
                N_AddDelta(networks[0],networks[i]);
            }
            N_AverageDelta(networks[0],threadsCount);

            for (size_t i = 0; i < threadsCount-1; i++)
            {
                pthread_mutex_unlock(mutex + i);
            }
            *n->accumulator = 0;
            
            N_Update(networks[0],learningRate);
            i += auxBatchSize;
        }
        //Automatically save the model at each model, only works for Mnist at the moment
        Reco_Save(networks[0],dataset,testSet);


    }
    for (size_t i = 0; i < threadsCount - 1; i++)
    {
        pthread_join(threads[i],NULL);
    }
    free(threads);
    for (size_t i = 1; i < threadsCount; i++)
    {
        free(networks[i]);
    }
    free(networks);
    
    free(args);
    free(progressBar);
}


void N_BackPropagate(Network* n, Matrix* delta)
{
    for (int i = n->layersCount - 1; i > 0; i--)
    {
        delta = n->layers[i]->BackPropagation(n->layers[i]->layerPtr,n->layers[i - 1]->outputs, delta);
    }
}



void N_Update(Network* n, float learningRate)
{
    for (size_t i = 1; i < n->layersCount; i++)
    {
        n->layers[i]->AdjustWeigths(n->layers[i]->layerPtr,learningRate,n->accumulator,n->lambda);
    }
    
}

void N_AddLayer(Network* n, Layer* l)
{
    if(n->layersCount == 0)
    {
        n->layers = (Layer**)malloc(sizeof(Layer*));
        n->layers[0] = l;
        n->layersCount++;
        return;
    }
    n->layersCount++;
    n->layers = (Layer**)realloc(n->layers,sizeof(Layer*) * n->layersCount);
    n->layers[n->layersCount - 1] = l;
    
}

void N_Save(Network* n, const char* path)
{
    FILE* file = fopen(path,"wb");
    if(file == NULL)
    {
        printf("File can't be open !\n");
        exit(-1);
    }
    fwrite(&n->layersCount,sizeof(size_t),1,file);
    for (size_t i = 0; i < n->layersCount; i++)
    {
        fwrite(&n->layers[i]->layerType,sizeof(unsigned char),1,file);
        n->layers[i]->Save(n->layers[i]->layerPtr,file);
    }
    fclose(file);
}

Network* N_Load(const char* path)
{
    FILE* file = fopen(path,"rb");
    Network* network = (Network*)malloc(sizeof(Network));
    fread(&network->layersCount,sizeof(size_t),1,file);
    network->layers = (Layer**)malloc(sizeof(Layer*) * network->layersCount);
    for (size_t i = 0; i < network->layersCount; i++)
    {
        unsigned char layerType;
        fread(&layerType,sizeof(unsigned char),1,file);
        switch (layerType)
        {
        case 0:
            network->layers[i] = I_Load(file);
            break;
        case 1:
            network->layers[i] = FCL_Load(file);
            break;
        case 2:
            network->layers[i] = Drop_Load(file);
            break;
        case 3:
            network->layers[i] = MaxPool_Load(file);
            break;
        case 4:
            network->layers[i] = ReshapeLayer_Load(file);
            break;
        case 5:
            network->layers[i] = Conv_Load(file);
            break;
        default:
            printf("layer type : %i\n",layerType);
            printf("Unknown layer type\n");
            exit(-1);
            break;
        }
    }
    fclose(file);
    return network;
}


void N_AddDelta(Network* n, Network* n2)
{
    for (size_t i = 1; i < n->layersCount; i++)
    {
        n->layers[i]->AddDelta(n->layers[i]->layerPtr,n2->layers[i]->layerPtr);
    }
    
}

void N_AverageDelta(Network* n, const size_t size)
{
    for (size_t i = 1; i < n->layersCount; i++)
    {
        n->layers[i]->AverageDelta(n->layers[i]->layerPtr,size);
    }
}


void N_ShuffleDataset(Dataset* dataset)
{
    for (size_t i = 0; i < dataset->size - 1; i++)
    {
        Matrix* tmp = dataset->data[0][i];
        Matrix* tmp2 = dataset->data[1][i];

        size_t random = rand() % (dataset->size - i) + i;

        dataset->data[0][i] = dataset->data[0][random];
        dataset->data[1][i] = dataset->data[1][random];

        dataset->data[0][random] = tmp;
        dataset->data[1][random] = tmp2;
    }
}


void N_Print(Network* n)
{
    int parametersCount = 0;
    printf("Network : \n");
    for (size_t i = 0; i < n->layersCount; i++)
    {
        PrintLine();
        n->layers[i]->Print(n->layers[i]->layerPtr,&parametersCount);
    }
    PrintLine();
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),"Total parameters : %i",parametersCount);
    PrintCentered(buffer);
    PrintLine();
    
}


void N_Reset(Network* n)
{
    for (size_t i = 0; i < n->layersCount; i++)
    {
        if(n->layers[i]->layerType == 4)
        {
            ReshapeLayer_Reset(n->layers[i]->layerPtr);
        }
    }
}