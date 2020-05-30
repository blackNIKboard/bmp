#include "image.h"
Image::Image() : rgbQuad(NULL)
{
}

Image::Image(const char mode, const unsigned short bCount, const int width, const int height)
{
    infoHeader.bitCount = bCount;
    infoHeader.width = width;
    infoHeader.height = height;
    rgbQuad = new RGBQUAD *[infoHeader.height];
    for (int i = 0; i < infoHeader.height; ++i)
        rgbQuad[i] = new RGBQUAD[infoHeader.width];

    for (int i = 0; i < infoHeader.height; ++i)
        for (int j = 0; j < infoHeader.width; ++j)
        {
            rgbQuad[i][j].red = mode;
            rgbQuad[i][j].green = mode;
            rgbQuad[i][j].blue = mode;
            rgbQuad[i][j].reserved = 0;
        }
}
Image &Image::operator=(const Image &other)
{
    assert(infoHeader.sizeImage == 0);

    for (int i = 0; i < infoHeader.height; ++i)
    {
        delete rgbQuad[i];
    }
    delete[] rgbQuad;

    infoHeader = other.infoHeader;
    rgbQuad = new RGBQUAD *[infoHeader.height];
    for (int i = 0; i < infoHeader.height; ++i)
    {
        rgbQuad[i] = new RGBQUAD[infoHeader.width];
    }

    for (int i = 0; i < infoHeader.height; ++i)
    {
        for (int j = 0; j < infoHeader.width; ++j)
        {
            rgbQuad[i][j] = other.rgbQuad[i][j];
        }
    }
    return *this;
}
Image::Image(const Image &other)
{
    infoHeader = other.infoHeader;
    rgbQuad = new RGBQUAD *[infoHeader.height];
    for (int i = 0; i < infoHeader.height; ++i)
    {
        rgbQuad[i] = new RGBQUAD[infoHeader.width];
    }
    for (int i = 0; i < infoHeader.height; ++i)
    {
        for (int j = 0; j < infoHeader.width; ++j)
        {
            rgbQuad[i][j] = other.rgbQuad[i][j];
        }
    }
}
Image::Image(const char *filename)
{
    loadImage(filename);
}
Image::~Image()
{
    for (int i = 0; i < infoHeader.height; ++i)
    {
        delete rgbQuad[i];
    }
    delete[] rgbQuad;
}

int Image::loadImage(const char *filename)
{
    BITMAPFILEHEADER bfh;
    FILE *file;
    file = fopen(filename, "rb");

    assert(file != NULL && "Error with opening a file");

    int tryToRead = 0;
    tryToRead = fread(&bfh, sizeof(BITMAPFILEHEADER) - 2, 1, file);

    assert(tryToRead != 0 && "Reading error. Your file is not .bmp");

    tryToRead = fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

    assert(tryToRead != 0 && "Reading error");

    RGBTRIPLE rgbTriple;
    rgbQuad = new RGBQUAD *[infoHeader.height];
    for (int i = 0; i < infoHeader.height; ++i)
    {
        rgbQuad[i] = new RGBQUAD[infoHeader.width];
    }
    int alignment = 4 - (infoHeader.width * 3) % 4;
    for (int i = 0; i < infoHeader.height; ++i)
    {
        if (infoHeader.bitCount == 24)
        {
            for (int j = 0; j < infoHeader.width; ++j)
            {
                fread(&rgbTriple, sizeof(RGBTRIPLE), 1, file);
                rgbQuad[i][j].red = rgbTriple.red;
                rgbQuad[i][j].green = rgbTriple.green;
                rgbQuad[i][j].blue = rgbTriple.blue;
                rgbQuad[i][j].reserved = 0;
            }
        }
        else if (infoHeader.bitCount == 32)
        {
            fread(rgbQuad[i], sizeof(RGBQUAD), infoHeader.width, file);
        }
        if (alignment != 4)
        {
            fseek(file, alignment, SEEK_CUR);
        }
    }
    return 1;
}

void Image::writeImage(const char *filename)
{
    FILE *file;

    file = fopen(filename, "wb");
    assert(file != NULL && "Error with opening a file");
    
    BITMAPFILEHEADER bfh(infoHeader.bitCount, infoHeader.width, infoHeader.height);
    char buf = 0;
    int alignment = 4 - (infoHeader.width * 3) % 4;
    fwrite(&bfh, sizeof(BITMAPFILEHEADER) - 2, 1, file);
    fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    RGBTRIPLE rgbTriple;
    for (int i = 0; i < infoHeader.height; i++)
    {
        if (infoHeader.bitCount == 24)
        {
            for (int j = 0; j < infoHeader.width; ++j)
            {
                rgbTriple.red = rgbQuad[i][j].red;
                rgbTriple.green = rgbQuad[i][j].green;
                rgbTriple.blue = rgbQuad[i][j].blue;
                fwrite(&rgbTriple, sizeof(RGBTRIPLE), 1, file);
            }
        }
        else if (infoHeader.bitCount == 32)
        {
            fwrite(rgbQuad[i], sizeof(RGBQUAD), infoHeader.width, file);
        }
        if (alignment != 4)
        {
            fwrite(&buf, 1, alignment, file);
        }
    }
}