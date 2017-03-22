// MuPdfConsole.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>

#include "PdfDocument.h"
#include "MuPdfApi.h"

std::vector<unsigned char> readFile(const char* filename)
{
	// open the file:
	std::streampos fileSize;
	std::ifstream file(filename, std::ios::binary);

	// get its size:
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	// read the data:
	std::vector<unsigned char> fileData(fileSize);
	file.read((char*)&fileData[0], fileSize);
	return fileData;
}

int main()
{
	auto pdf = "C:\\Users\\katiehan\\Documents\\14_Spring2017_EpipolarGeometryAndStructureFromMotion.pdf";
	auto bytes = readFile(pdf);
	auto bytesArr = &bytes[0];
	auto doc = Open(bytesArr, bytes.size());

	ActivateDocument(doc);
	for (int i = 0; i < GetNumPages(); i++)
	{		
		GotoPage(i);
		//auto numbytes = RenderPage(GetPageWidth(), GetPageWidth());
	//	auto buf = GetBuffer();

		//std::cout << "rendering page: " << i << std::endl; 

	}
	
	Dispose(doc);

	return 0;
};




