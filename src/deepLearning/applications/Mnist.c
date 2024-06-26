#include "deepLearning/applications/Mnist.h"
#include "deepLearning/Network.h"
#include "deepLearning/layer/FCL.h"
#include "deepLearning/layer/InputLayer.h"
#include "deepLearning/Dataset.h"
#include "deepLearning/Activation.h"
#include "matrix/BlockMatrix.h"
#include "imageProcessing/ImageTransformation.h"
#include "deepLearning/layer/ConvLayer.h"
#include "deepLearning/layer/MaxPoolLayer.h"
#include "deepLearning/layer/Dropout.h"
#include "deepLearning/layer/ReshapeLayer.h"
#include "tools/FileTools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <dirent.h>
#include <stdint.h>




const char* MNIST_DATA_PATH = "datasets/train-images.idx3-ubyte";
const char* MNIST_LABEL_PATH = "datasets/train-labels.idx1-ubyte";
const char* MNIST_TEST_DATA_PATH = "datasets/t10k-images.idx3-ubyte";
const char* MNIST_TEST_LABEL_PATH = "datasets/t10k-labels.idx1-ubyte";
const char* SUDOKU_DATA_PATH = "datasets/datasetCreation/processed/all_digits_images.bin";
const char* HANDWRITTEN_DATA_PATH = "datasets/digits.data";

Matrix* temp = NULL;

void AddNoise(Matrix* input, Matrix* output)
{
    M_RotateImage_1D(input,temp,(rand() % 15) - 7.5,28);
    M_Zoom(temp,output,((rand() % 20) / 100.0) + 1);
   
}


void Mnist_Train_ConvLayers()
{
    printf("Strating training ...\n");
    Dataset* trainDataset = LoadCombinedTrainDataset();

    printf("Size of the dataset : %zu\n",trainDataset->size); 
    for (size_t i = 0; i < trainDataset->size; i++)
    {
        trainDataset->data[0][i]->rows = 28;
        trainDataset->data[0][i]->cols = 28;
    }
    
    


    Network* network = N_CreateNetwork();
    N_AddLayer(network, I_Create_2D(LS_Create3D(28, 28, 1)));
    N_AddLayer(network, Conv_Create(LS_Create3D(5, 5, 64))); 
    N_AddLayer(network, MaxPool_Create(2));
    N_AddLayer(network, Conv_Create(LS_Create3D(3, 3, 64))); 
    N_AddLayer(network, MaxPool_Create(2));
    N_AddLayer(network, Flatten_Create());
    N_AddLayer(network, FCL_Create(128, ReLU()));
    N_AddLayer(network, FCL_Create(10, Softmax()));

    N_Compile(network, CE_Create());

    N_Print(network);

    int num_of_logical_processors = sysconf(_SC_NPROCESSORS_ONLN);

    printf("Number of logical processors : %d\n",num_of_logical_processors);

    N_Train(network,trainDataset,1000,128,60,num_of_logical_processors,0.001);

    Dataset* testDataset = LoadMnist(MNIST_TEST_DATA_PATH,MNIST_TEST_LABEL_PATH,2051,2049);

    float trainAccuray = TestAccuracy(network,trainDataset);
    printf("🍔 Train accuracy : %.2f%% 🍔\n",trainAccuray);

    float testAccuracy = TestAccuracy(network,testDataset);
    printf("🍔 Test accuracy : %.2f%% 🍔\n",testAccuracy);

    N_Free(network);
}


void Mnist_Train()
{
    temp = M_Create_2D(784,1);
    printf("Starting Training ... \n");
    Dataset* trainDataset = LoadMnist(MNIST_DATA_PATH,MNIST_LABEL_PATH,2051,2049);

    Network* network = N_CreateNetwork();
    N_AddLayer(network, I_Create(784));
    N_AddLayer(network, FCL_Create(128,ReLU()));
    N_AddLayer(network, FCL_Create(64, ReLU())); 
    //N_AddLayer(network, FCL_Create(16, ReLU()));
    //N_AddLayer(network, FCL_Create(256, ReLU())); 

    N_AddLayer(network, FCL_Create(10,Softmax()));
    
    N_Compile_L2(network,CE_Create(),0.0001);

    int num_of_logical_processors = sysconf(_SC_NPROCESSORS_ONLN);

    printf("Number of logical processors : %d\n",num_of_logical_processors);


    N_Train(network,trainDataset,1000,128,20,num_of_logical_processors,0.001);


    Dataset* testDataset = LoadMnist(MNIST_TEST_DATA_PATH,MNIST_TEST_LABEL_PATH,2051,2049);

    
    float trainAccuray = TestAccuracy(network,trainDataset);
    printf("🍔 Train accuracy : %.2f%% 🍔\n",trainAccuray);
    float testAccuracy = TestAccuracy(network,testDataset);
    printf("🍔 Test accuracy : %.2f%% 🍔\n",testAccuracy);

    

    char* path = malloc(sizeof(char) * 300);

    sprintf(path,"./models/recognition/model_%.4f.model",testAccuracy);
    N_Save(network,path);
    free(path);
    Dataset_Free(trainDataset);
    Dataset_Free(testDataset);
    N_Free(network);
}

void Reco_Save(Network* network,Dataset* trainSet,Dataset* testSet)
{
    char* path = malloc(sizeof(char) * 300);
    size_t trainSetSize = trainSet->size;
    trainSet->size = trainSet->size < 10000 ? trainSet->size : 10000;
    float trainAccuray = TestAccuracy(network,trainSet);
    printf("🍔 Train accuracy : %.2f%% 🍔\n",trainAccuray);
    trainSet->size = trainSetSize;
    float testAccuracy = TestAccuracy(network,testSet);
    printf("🍔 Test accuracy : %.2f%% 🍔\n",testAccuracy);
    sprintf(path,"./models/recognition/model_%.4f.model",testAccuracy);
    N_Save(network,path);
    free(path);
}

void Rotation_Train()
{
    printf("Starting Training ... \n");
    Dataset* MnistDataset = LoadMnist(MNIST_DATA_PATH,MNIST_LABEL_PATH,2051,2049);
    Dataset* trainDataset = (Dataset*)malloc(sizeof(Dataset));
    trainDataset->size = MnistDataset->size;
    trainDataset->data = (Matrix***)malloc(sizeof(Matrix**) * 2);
    trainDataset->data[0] = (Matrix**)malloc(sizeof(Matrix*) * trainDataset->size);
    trainDataset->data[1] = (Matrix**)malloc(sizeof(Matrix*) * trainDataset->size);
    for (size_t i = 0; i < trainDataset->size; i++)
    {
        size_t rotation = rand() % 4;
        float rot = rotation * 90;
        trainDataset->data[0][i] = M_RotateImage_1DI(MnistDataset->data[0][i],rot,28);
        trainDataset->data[1][i] = S_LabelToMatrix(rotation,4);
    }
    Dataset_Free(MnistDataset);
    
    
    

    Network* network = N_CreateNetwork();
    N_AddLayer(network, I_Create(784));
    N_AddLayer(network, FCL_Create(256,ReLU()));
    N_AddLayer(network, FCL_Create(256,ReLU()));
    N_AddLayer(network, FCL_Create(256,ReLU()));
    //N_AddLayer(network, FCL_Create(256,ReLU()));
    N_AddLayer(network, FCL_Create(4,Softmax()));
    
    N_Compile(network,CE_Create());

    int num_of_logical_processors = sysconf(_SC_NPROCESSORS_ONLN);

    printf("Number of logical processors : %d\n",num_of_logical_processors);

    trainDataset->size = 50000;

    N_Train(network,trainDataset,1000,128,10,1,0.001);

    float trainAccuray = TestAccuracy(network,trainDataset);
    printf("🍔 Train accuracy : %.2f%% 🍔\n",trainAccuray);


    trainDataset->size = 10000;
    trainDataset->data[0] += 50000;
    trainDataset->data[1] += 50000;

    float testAccuracy = TestAccuracy(network,trainDataset);
    printf("🍔 Test accuracy : %.2f%% 🍔\n",testAccuracy);

    char* path = malloc(sizeof(char) * 3000);
    sprintf(path,"./models/orientation/model_%.4f.model",testAccuracy);

    N_Save(network,path);


}
Dataset* LoadTestMnist()
{
    Dataset* testDataset = LoadMnist(MNIST_TEST_DATA_PATH,MNIST_TEST_LABEL_PATH,2051,2049);
    return testDataset;
}

void Mnist_Load(const char* dataFilename)
{
    Network* network = N_Load(dataFilename);
    N_Compile(network,CE_Create());
    Dataset* testDataset = LoadMnist(MNIST_TEST_DATA_PATH,MNIST_TEST_LABEL_PATH,2051,2049);
    float testAccuracy = TestAccuracy(network,testDataset);
    printf("🍔 Test accuracy : %.2f%% 🍔\n",testAccuracy);
    N_Free(network);
    Dataset_Free(testDataset);
}


int ReverseInt(int i)
{
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;
    return ((int) c1 << 24) + ((int) c2 << 16) + ((int) c3 << 8) + c4;
}

Matrix* S_LabelToMatrix(char label,size_t size)
{
    Matrix* res = M_Create_2D(size,1);
    for (int i = 0; i < size; i++)
    {
        if(i == label)
        {
            res->data[i] = 1;
        }
        else
        {
            res->data[i] = 0;
        }
    }
    return res;
    
}


Dataset* LoadCombinedTrainDataset()
{
    Dataset* mnistTrain = LoadMnist(MNIST_DATA_PATH,MNIST_LABEL_PATH,2051,2049);
    printf("Size of the mnist dataset : %zu\n",mnistTrain->size);
    Dataset* sudokuTrain = LoadSudokuDigitDataset(SUDOKU_DATA_PATH,1);
    printf("Size of the sudoku dataset : %zu\n",sudokuTrain->size);
    Dataset* handMadeSudoku = LoadHandWritten(HANDWRITTEN_DATA_PATH);
    printf("Size of the hand made dataset : %zu\n",handMadeSudoku->size);


    Dataset* res = (Dataset*)malloc(sizeof(Dataset));
    res->size = mnistTrain->size + sudokuTrain->size + handMadeSudoku->size;
    res->data = (Matrix***)malloc(sizeof(Matrix**) * 2);
    res->data[0] = (Matrix**)malloc(sizeof(Matrix*) * res->size);
    res->data[1] = (Matrix**)malloc(sizeof(Matrix*) * res->size);
    for (size_t i = 0; i < mnistTrain->size; i++)
    {
        res->data[0][i] = mnistTrain->data[0][i];
        res->data[1][i] = mnistTrain->data[1][i];
    }
    for (size_t i = 0; i < sudokuTrain->size; i++)
    {
        res->data[0][i + mnistTrain->size] = sudokuTrain->data[0][i];
        res->data[1][i + mnistTrain->size] = sudokuTrain->data[1][i];
    }
    for (size_t i = 0; i < handMadeSudoku->size; i++)
    {
        res->data[0][i + mnistTrain->size + sudokuTrain->size] = handMadeSudoku->data[0][i];
        res->data[1][i + mnistTrain->size + sudokuTrain->size] = handMadeSudoku->data[1][i];
    }

    printf("Size of the dataset : %zu\n",res->size);

    free(mnistTrain->data[0]);
    free(mnistTrain->data[1]);
    free(mnistTrain->data);
    free(mnistTrain);
    free(sudokuTrain->data[0]);
    free(sudokuTrain->data[1]);
    free(sudokuTrain->data);
    free(sudokuTrain);
    free(handMadeSudoku->data[0]);
    free(handMadeSudoku->data[1]);
    free(handMadeSudoku->data);
    return res;
}

Dataset* LoadMnist(const char* dataFilename,const char* labelFilename, int magicNumberData, int magicNumberLabel)
{
    
    FILE* dataFile;
    FILE* labelFile;
    int* Ibuffer = (int*)malloc(sizeof(int));
    unsigned char* Cbuffer = (unsigned char*)malloc(sizeof(unsigned char));

    dataFile = fopen(dataFilename, "rb");
    labelFile = fopen(labelFilename, "rb");
    
    if(dataFile == NULL)
    {
        printf("file can't be open !\n");
        exit(-1);
    }

    if(labelFile == NULL)
    {
        printf("Label file cannot be open !\n");
        exit(-1);
    }

    CheckRead(fread(Ibuffer,sizeof(int),1,dataFile));

    if (ReverseInt(Ibuffer[0]) != magicNumberData)
    {
        printf("Invalid magic number !\n");
        return NULL;
    }

    CheckRead(fread(Ibuffer,sizeof(int),1,labelFile));

    if (ReverseInt(Ibuffer[0]) != magicNumberLabel)
    {
        printf("Invalid magic number !\n");
        return NULL;
    }


    CheckRead(fread(Ibuffer,sizeof(int),1,dataFile));
    size_t numberOfMatrix = ReverseInt(*Ibuffer);



    CheckRead(fread(Ibuffer,sizeof(int),1,dataFile));
    size_t n_rows = ReverseInt(*Ibuffer);
    
    
    CheckRead(fread(Ibuffer,sizeof(int),1,dataFile));
    size_t n_cols = ReverseInt(*Ibuffer);
    
    
    Matrix*** res = (Matrix***)malloc(sizeof(Matrix**) * 2);
    res[0] = (Matrix**)malloc(sizeof(Matrix*) * numberOfMatrix);
    res[1] = (Matrix**)malloc(sizeof(Matrix*) * numberOfMatrix);

    CheckRead(fread(Ibuffer,sizeof(int),1,labelFile));

    size_t i = 0;
    while (i < numberOfMatrix)
    {
        CheckRead(fread(Cbuffer,sizeof(unsigned char),1,labelFile));
        res[1][i] = S_LabelToMatrix(Cbuffer[0],10);
        res[0][i] = M_Create_2D(n_cols * n_rows,1);

        size_t j = 0;
        while (j < n_rows * n_cols)
        {
            CheckRead(fread(Cbuffer,sizeof(char),1,dataFile));
            res[0][i]->data[j] = (float)(*Cbuffer) / (float)255;
            j++;
        }
        i++;
    }
    
    Dataset* dataset = (Dataset*)malloc(sizeof(Dataset));
    dataset->data = res;
    dataset->size = numberOfMatrix;


    fclose(dataFile);
    fclose(labelFile);
    free(Ibuffer);
    free(Cbuffer);


    

    return dataset;
    
}

char S_MatrixToLabel(const Matrix* m)
{
    float max = 0;
    int index = 0;
    for (char i = 0; i < m->rows; i++)
    {
        float value = m->data[i];
        if(value > max)
        {
            max = value;
            index = i;
        }
    }
    return (char)index;
    
}

float TestAccuracy(Network* network, Dataset* dataset)
{
    size_t correct = 0;
    for (size_t i = 0; i < dataset->size; i++)
    {
        const Matrix* res = N_Process(network,dataset->data[0][i]);
        char label = S_MatrixToLabel(res);
        if(label == S_MatrixToLabel(dataset->data[1][i]))
        {
            correct++;
        }
    }
    return (float)correct * 100 / (float)dataset->size;
}

int filter(const struct dirent* name)
{
    if(strstr(name->d_name,".model") != NULL)
    {
        return 1;
    }
    return 0;
}

char* FindBestModel(char* folder)
{
    struct dirent **namelist;
    int n = scandir(folder, &namelist, filter, alphasort);
    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }
    if(n == 0)
    {
        printf("No model found !\n");
        exit(-1);
    }
    char* bestModel = namelist[n-1]->d_name;
    //free
    for (size_t i = 0; i < n-1; i++)
    {
        free(namelist[i]);
    }
    free(namelist);

    return bestModel;

}

Network* LoadBestRecognitionModel()
{
    char* bestModel = FindBestModel("./models/recognition");
    char path[1024];
    snprintf(path, sizeof(path), "./models/recognition/%s",bestModel);

    printf("Loading model : %s\n",path);
    Network* network = N_Load(path);
    N_Compile(network,CE_Create());
    return network;
}



Dataset* LoadSudokuDigitDataset()
{
    Matrix*** dataset = (Matrix***)malloc(sizeof(Matrix**) * 2);
    FILE* datasetFile = fopen(SUDOKU_DATA_PATH,"rb");

    if(datasetFile == NULL)
    {
        printf("Dataset file cannot be open !\n");
        exit(-1);
    }
    __uint32_t datasetSize;
    size_t read = fread(&datasetSize,sizeof(datasetSize),1,datasetFile);
    if(read != 1)
    {
        printf("Error while reading dataset size !\n");
        exit(-1);
    }
    printf("Dataset size : %u\n",datasetSize);

    dataset[0] = (Matrix**)malloc(sizeof(Matrix*) * datasetSize);
    dataset[1] = (Matrix**)malloc(sizeof(Matrix*) * datasetSize);

    for (size_t i = 0; i < datasetSize; i++)
    {
        dataset[0][i] = M_Create_2D(28 * 28,1);
        uint8_t label;
        read = fread(&label,sizeof(label),1,datasetFile);
        if(read != 1)
        {
            printf("Error while reading dataset size !\n");
            exit(-1);
        }
        dataset[1][i] = S_LabelToMatrix(label,10);

        for (size_t j = 0; j < 784; j++)
        {
            uint8_t value;
            read = fread(&value,sizeof(value),1,datasetFile);
            if(read != 1)
            {
                printf("Error while reading dataset !\n");
                exit(-1);
            }
            dataset[0][i]->data[j] = value / 255.0;
        }
    }

    Dataset* res = (Dataset*)malloc(sizeof(Dataset));
    res->data = dataset;
    res->size = datasetSize;

    return res;
}
void SaveDigit(const Matrix* matrix, uint8_t label, FILE* file)
{
    fwrite(&label,sizeof(label),1,file);
    M_Save(matrix,file);
    printf("%d saved \n",label);

}

Dataset* LoadHandWritten(const char* path)
{
    Matrix*** dataset = (Matrix***)malloc(sizeof(Matrix**) * 2);
    FILE* datasetFile = fopen(path,"rb");

    if(datasetFile == NULL)
    {
        printf("Dataset file cannot be open !\n");
        exit(-1);
    }
    __uint32_t datasetSize;
    size_t read;


    //Get the length of the file in bytes 
    size_t length;
    fseek(datasetFile, 0, SEEK_END);
    length = ftell(datasetFile);
    fseek(datasetFile, 0, SEEK_SET);
    datasetSize = length / ((M_SaveSizeDim(28,28,1) + 1));


    dataset[0] = (Matrix**)malloc(sizeof(Matrix*) * datasetSize);
    dataset[1] = (Matrix**)malloc(sizeof(Matrix*) * datasetSize);

    for (size_t i = 0; i < datasetSize; i++)
    {
        uint8_t label;
        read = fread(&label,sizeof(label),1,datasetFile);
        if(read != 1)
        {
            printf("Error while reading dataset size !\n");
            exit(-1);
        }
        dataset[1][i] = S_LabelToMatrix(label,10);

        dataset[0][i] =  M_Load(datasetFile);
    }

    Dataset* res = (Dataset*)malloc(sizeof(Dataset));
    res->data = dataset;
    res->size = datasetSize;

    return res;
}