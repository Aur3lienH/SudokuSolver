#include "imageProcessing/Binarisation.h"
#include <stdlib.h>
#include "matrix/Matrix.h"
#include "imageProcessing/Preprocessing.h"
#include <math.h>



Matrix* Binarisation(Matrix* input, float threshold)
{
    Matrix* output = M_Create_2D(input->cols, input->rows);
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            if (input->data[j + i * input->cols] > threshold)
            {
                output->data[j + i * input->cols] = 1.0f;
            }
            else
            {
                output->data[j + i * input->cols] = 0;
            }
        }
    }
    return output;
}


float M_Mean(Matrix* input)
{
    float mean = 0;
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            mean += input->data[j + i * input->cols];
        }
    }
    return mean / (input->rows * input->cols);
}

float M_Variance(Matrix* input)
{
    float mean = M_Mean(input);
    float variance = 0;
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            variance += (input->data[j + i * input->cols] - mean) * (input->data[j + i * input->cols] - mean);
        }
    }
    return variance / (input->rows * input->cols);
}

void M_Inverse(Matrix* input)
{
    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            input->data[j + i * input->cols] = 1 - input->data[j + i * input->cols];
        }
    }
}


// Function to calculate the mean of a grayscale image
int calculate_mean(int histogram[], int num_pixels) {
  int sum = 0;
  for (int i = 0; i < 256; i++) {
    sum += i * histogram[i];
  }
  return sum / num_pixels;
}

double calculateMean(int* histogram, int startThreshold, int endThreshold, int* _numPixels)
{
    double sum = 0;
    double numPixels = 0;
    for (size_t i = startThreshold; i < endThreshold; i++)
    {
        sum += i * histogram[i];
        numPixels += histogram[i];
    }
    if(numPixels == 0) return 0;
    *_numPixels = numPixels;
    return sum / numPixels;
}

// Function to calculate the variance of a grayscale image for a specific threshold
double calculateDifferenceOfMean(int* histogram, int num_pixels, int threshold) {
    int numberOfPixels1, numberOfPixels2;
    double mean1 = calculateMean(histogram, 0, threshold, &numberOfPixels1);
    double mean2 = calculateMean(histogram, threshold, 256, &numberOfPixels2);
    return (mean1 - mean2) * (mean1 - mean2) * sqrt(numberOfPixels1 * numberOfPixels2);
}

double calculateDifferenceOfMean3(int* histogram, int num_pixels, int threshold1, int threshold2)
{
    int numberOfPixels1, numberOfPixels2, numberOfPixels3;
    double mean1 = calculateMean(histogram, 0, threshold1, &numberOfPixels1);
    double mean2 = calculateMean(histogram, threshold1, threshold2, &numberOfPixels2);
    double mean3 = calculateMean(histogram, threshold2, 256, &numberOfPixels3);
    return (mean1 - mean2) * (mean1 - mean2) * sqrt(numberOfPixels1 * numberOfPixels2) + (mean2 - mean3) * (mean2 - mean3) * sqrt(numberOfPixels2 * numberOfPixels3);
}

// Function to find the optimal threshold using Otsu's method
int otsu_threshold(int* histogram, int num_pixels) {
  int bestThreshold = 0;
  double bestVariance = 0;
  for (size_t i = 0; i < 256; i++)
  {
      double variance = calculateDifferenceOfMean(histogram, num_pixels, i);
      if(variance >= bestVariance)
      {
          bestVariance = variance;
          bestThreshold = i;
      }
  }
  return bestThreshold;
}

int* M_GetHistogram(Matrix* input)
{
    int* histogram = (int*)malloc(sizeof(int) * 256);
    for (size_t i = 0; i < 256; i++)
    {
        histogram[i] = 0;
    }

    for (size_t i = 0; i < input->rows; i++)
    {
        for (size_t j = 0; j < input->cols; j++)
        {
            histogram[(size_t)(input->data[j + i * input->cols] * 255)]++;
        }
    }
    return histogram;
}

float M_GetOtsu(Matrix* input)
{
    int* histogram = M_GetHistogram(input);
    return otsu_threshold(histogram, input->rows * input->cols) / 255.0f;
}


float M_GetOtsu3(Matrix* input)
{
    int* histogram = M_GetHistogram(input);
    int bestThreshold = 0;
    double bestVariance = 0;

    for (size_t i = 0; i < 256; i++)
    {
        for (size_t j = i; j < 256; j++)
        {
            double variance = calculateDifferenceOfMean3(histogram, input->rows * input->cols, i, j);
            if(variance >= bestVariance)
            {
                bestVariance = variance;
                bestThreshold = j;
            }
        }
    }
    return bestThreshold / 255.0f;
    
}





float findThreshold(Matrix* input)
{
    float mean = M_Mean(input);
    return mean;
}

Matrix* M_OptimalBinarisation(Matrix* input)
{
    Matrix* gaussian = GaussianBlur(input, 1.0f);

    float otsu = M_Mean(gaussian);
    //printf("Otsu: %f\n", otsu);
    //Mean adaptive threshold
    Matrix* binarised = Binarisation(gaussian, otsu);
    //Variance adaptive threshold

    M_Inverse(binarised);
    return gaussian;
}




