#include "network/DigitSend.h"
#include "network/Protocols.h"


void SendDigit(Matrix** matrix, char** digit, size_t matrixCount)
{
    size_t len = sizeof(unsigned char) * (*matrix)->cols * (*matrix)->rows * matrixCount + sizeof(char) * matrixCount + 1;
    unsigned char* data = malloc(len);
    
    for (size_t i = 0; i < matrixCount; i++)
    {
        for (size_t j = 0; j < (*matrix)->cols * (*matrix)->rows; j++)
        {
            data[j + i * (*matrix)->cols * (*matrix)->rows] = (unsigned char)((*matrix)->data[j] * 255);
        }
    }
    for (size_t i = 0; i < matrixCount; i++)
    {
        data[(*matrix)->cols * (*matrix)->rows * matrixCount + i] = (*digit)[i];
    }
    data[len - 1] = '\0';
    
    //Send data
    Net_Send(data, len);

    free(data);
    
}