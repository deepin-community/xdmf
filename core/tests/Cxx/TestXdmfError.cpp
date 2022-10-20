#include <iostream>
#include <stdlib.h>
#include <XdmfArray.hpp>
#include <XdmfArrayType.hpp>
#include <XdmfError.hpp>

int main(int, char **)
{
	
	shared_ptr<XdmfArray> writtenArray = XdmfArray::New();
	std::vector<unsigned int> dimensionVector;
	dimensionVector.push_back(20);
//	dimensionVector.push_back(5);
//	dimensionVector.push_back(4);
	writtenArray->initialize<int>(dimensionVector);
	for (int i = 0; i < 20; i++)
	{
		writtenArray->insert(i, i + 1);
	}

	shared_ptr<XdmfArray> readArray = XdmfArray::New();
	std::vector<unsigned int> readDimensionVector;
	readDimensionVector.push_back(6);
	readDimensionVector.push_back(4);
	readArray->initialize<int>(readDimensionVector);

	std::vector<unsigned int> writeStarts;
	writeStarts.push_back(0);
	writeStarts.push_back(0);
	std::vector<unsigned int> writeStrides;
	writeStrides.push_back(2);
	writeStrides.push_back(2);
	std::vector<unsigned int> writeDim;
	writeDim.push_back(3);
	writeDim.push_back(2);
	std::vector<unsigned int> readStarts;
	readStarts.push_back(0);
	readStarts.push_back(0);
	std::vector<unsigned int> readStrides;
	readStrides.push_back(2);
	readStrides.push_back(2);
	std::vector<unsigned int> readDim;
	readDim.push_back(3);
	readDim.push_back(2);

	try
	{	
		readArray->insert(readStarts, writtenArray, writeStarts, writeDim, readDim, readStrides, writeStrides);
	}
	catch (XdmfError e)
	{
		//a print statement is redundant since the error message is sent to cout
		//std::cout << "The error message is:\n" << e.what() << std::endl;
	}

	std::cout << "default error levels" << std::endl;

	try
	{
		XdmfError::message(XdmfError::FATAL, "throwing fatal");
	}
	catch (XdmfError e)
	{
		std::cout << "fatal caught" << std::endl;
	}

        try
        {
		XdmfError::message(XdmfError::WARNING, "throwing warning");
        }
        catch (XdmfError e)
        {
		std::cout << "warning caught" << std::endl;
        }

        try
        {
		XdmfError::message(XdmfError::DEBUG, "throwing debug");
        }
        catch (XdmfError e)
        {
		std::cout << "debug caught" << std::endl;
        }

	std::cout << std::endl;

	std::cout << "both levels set to FATAL" << std::endl;

	XdmfError::setSuppressionLevel(XdmfError::FATAL);
	XdmfError::setLevelLimit(XdmfError::FATAL);

        try
        {
                XdmfError::message(XdmfError::FATAL, "throwing fatal");
        }
        catch (XdmfError e)
        {
                std::cout << "fatal caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::WARNING, "throwing warning");
        }
        catch (XdmfError e)
        {
                std::cout << "warning caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::DEBUG, "throwing debug");
        }
        catch (XdmfError e)
        {
                std::cout << "debug caught" << std::endl;
        }

	std::cout << std::endl;

	std::cout << "both levels set to WARNING" << std::endl;

	XdmfError::setSuppressionLevel(XdmfError::WARNING);
	XdmfError::setLevelLimit(XdmfError::WARNING);

        try
        {
                XdmfError::message(XdmfError::FATAL, "throwing fatal");
        }
        catch (XdmfError e)
        {
                std::cout << "fatal caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::WARNING, "throwing warning");
        }
        catch (XdmfError e)
        {
                std::cout << "warning caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::DEBUG, "throwing debug");
        }
        catch (XdmfError e)
        {
                std::cout << "debug caught" << std::endl;
        }

	std::cout << std::endl;

	std::cout << "both levels set to DEBUG" << std::endl;

	XdmfError::setSuppressionLevel(XdmfError::DEBUG);
	XdmfError::setLevelLimit(XdmfError::DEBUG);

        try
        {
                XdmfError::message(XdmfError::FATAL, "throwing fatal");
        }
        catch (XdmfError e)
        {
                std::cout << "fatal caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::WARNING, "throwing warning");
        }
        catch (XdmfError e)
        {
                std::cout << "warning caught" << std::endl;
        }

        try
        {
                XdmfError::message(XdmfError::DEBUG, "throwing debug");
        }
        catch (XdmfError e)
        {
                std::cout << "debug caught" << std::endl;
        }

	return 0;
}
