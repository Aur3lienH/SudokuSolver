#include "imageProcessing/exifReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_assert.h>



__uint16_t read_2_bytes(FILE *file, int big_endian) {
    __uint8_t bytes[2];
    if (fread(bytes, 1, 2, file) != 2) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    if (big_endian) {
        return (bytes[0] << 8) | bytes[1];
    } else {
        return (bytes[1] << 8) | bytes[0];
    }
}

__uint32_t read_4_bytes(FILE *file, int big_endian) {
    __uint8_t bytes[4];
    if (fread(bytes, 1, 4, file) != 4) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    if (big_endian) {
        return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    } else {
        return (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];
    }
}


int getExifOrientation(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        return -1;
    }

     // Check for JPEG header


    __uint16_t header = read_2_bytes(file, 1);

    if (header != 0xFFD8) {
        fprintf(stderr, "Not a JPEG file\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Search for the Exif header
    __uint16_t marker, length;
    do {
        marker = read_2_bytes(file, 1);
        length = read_2_bytes(file, 1);
        if (marker == 0xFFE1) { // Exif header
            break;
        }
        fseek(file, length - 2, SEEK_CUR);
    } while (marker != 0xFFDA && marker != 0xFFD9);

    if (marker != 0xFFE1) {
        fprintf(stderr, "Exif header not found\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    char exifHeader[6];
    if (fread(exifHeader, 1, 6, file) != 6 || strncmp(exifHeader, "Exif\0\0", 6) != 0) {
        fprintf(stderr, "Invalid Exif header\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    // Read the TIFF header
    __uint16_t tiffHeader = read_2_bytes(file, 1);
    if (tiffHeader != 0x4949 && tiffHeader != 0x4D4D) {
        fprintf(stderr, "Invalid TIFF header\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    int big_endian = tiffHeader == 0x4D4D;

    if (read_2_bytes(file, big_endian) != 0x002A) {
        fprintf(stderr, "Invalid TIFF header\n");
        fclose(file);    
        return EXIT_FAILURE;
    }

    // Read the offset to the IFD
    __uint32_t ifdOffset = read_4_bytes(file, big_endian);
    fseek(file, ifdOffset - 8, SEEK_CUR);

    // Read the number of entries in the IFD
    __uint16_t numEntries = read_2_bytes(file, big_endian);

    // Search for the orientation tag
    for (int i = 0; i < numEntries; i++) {
        __uint16_t tag = read_2_bytes(file, big_endian);
        if (tag == 0x0112) { // Orientation tag
            read_2_bytes(file, big_endian); // Skip the type
            __uint32_t count = read_4_bytes(file, big_endian);
            if (count != 1) {
                fprintf(stderr, "Invalid orientation tag\n");
                fclose(file);
                return EXIT_FAILURE;
            }
            unsigned short orientation = read_2_bytes(file, big_endian);
            fclose(file);
            return orientation;
        } else {
            fseek(file, 10, SEEK_CUR); // Skip the rest of the entry
        }
    }

    fclose(file);
    return EXIT_FAILURE;

}