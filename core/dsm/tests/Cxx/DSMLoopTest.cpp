#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include "XdmfArray.hpp"
#include "XdmfArrayType.hpp"
#include "XdmfHDF5WriterDSM.hpp"
#include "XdmfHDF5ControllerDSM.hpp"

int main(int argc, char *argv[])
{
        int size, id, dsmSize;
        dsmSize = 64;//The total size of the DSM being created
        MPI_Comm comm = MPI_COMM_WORLD;

        MPI_Init(&argc, &argv);

        MPI_Comm_rank(comm, &id);
        MPI_Comm_size(comm, &size);


        std::vector<unsigned int> outputVector;

        shared_ptr<XdmfArray> testArray = XdmfArray::New();
        testArray->initialize<int>(0);

        shared_ptr<XdmfArray> testArray2 = XdmfArray::New();
        testArray2->initialize<int>(0);

        std::string newPath = "dsm";
        std::string newSetPath = "dataspace";
        std::string secondSetPath = "data2";

        // Change this to determine the number of cores used as servers
        unsigned int numServersCores = 2;
        // Change this to determine the size of the arrays generated when initializing
        unsigned int writeArraySize = 4;

        std::vector<unsigned int> writeStartVector;
        std::vector<unsigned int> writeStrideVector;
        std::vector<unsigned int> writeCountVector;
        std::vector<unsigned int> writeDataSizeVector;

        std::vector<unsigned int> readStartVector;
        std::vector<unsigned int> readStrideVector;
        std::vector<unsigned int> readCountVector;
        std::vector<unsigned int> readDataSizeVector;

        std::vector<unsigned int> readOutputCountVector;

        shared_ptr<XdmfArray> readArray = XdmfArray::New();
        readArray->initialize<int>(0);

        shared_ptr<XdmfArray> readArray2 = XdmfArray::New();
        readArray2->initialize<int>(0);

        shared_ptr<XdmfHDF5ControllerDSM> readController;
        shared_ptr<XdmfHDF5ControllerDSM> readOutputController;
        shared_ptr<XdmfHDF5ControllerDSM> writeController;

        shared_ptr<XdmfHDF5ControllerDSM> readController2;
        shared_ptr<XdmfHDF5ControllerDSM> readOutputController2;
        shared_ptr<XdmfHDF5ControllerDSM> writeController2;

        MPI_Comm workerComm;

        MPI_Group workers, dsmgroup;

        MPI_Comm_group(comm, &dsmgroup);
        int * ServerIds = (int *)calloc((numServersCores), sizeof(int));
        unsigned int index = 0;
        for(int i=size-numServersCores ; i <= size-1 ; ++i)
        {
                ServerIds[index++] = i;
        }

        MPI_Group_excl(dsmgroup, index, ServerIds, &workers);
        int testval = MPI_Comm_create(comm, workers, &workerComm);
        cfree(ServerIds);

        shared_ptr<XdmfHDF5WriterDSM> exampleWriter = XdmfHDF5WriterDSM::New(newPath, comm, dsmSize/numServersCores, size-numServersCores, size-1);

        exampleWriter->setMode(XdmfHeavyDataWriter::Hyperslab);

        //split out sub-comm for the worker cores
        //server cores will not progress to this point until after the servers are done running

        if (id < size - (int)numServersCores)
        {
                // Split the comm even further

                MPI_Comm readComm, writeComm;

                MPI_Group readingCores, writingCores;

                MPI_Comm_group(workerComm, &workers);
                int * ServerIds = (int *)calloc(((size - numServersCores) / 2), sizeof(int));
                unsigned int index = 0;
                for(int i=0 ; i < (int)((size - numServersCores) / 2) ; ++i)
                {
                        ServerIds[index++] = i;
                }

                MPI_Group_excl(workers, index, ServerIds, &readingCores);
                testval = MPI_Comm_create(workerComm, readingCores, &readComm);
                MPI_Group_incl(workers, index, ServerIds, &writingCores);
                testval = MPI_Comm_create(workerComm, writingCores, &writeComm);
                cfree(ServerIds);

                std::cout << "initializing" << std::endl;

                // Initialize values
                // Writer is first
                if (id < (int)((size - numServersCores) / 2))
                {
                        for (unsigned int i = 1; i <= writeArraySize; ++i)
                        {
                                testArray->pushBack(i*(id+1));
                        }

                        for (unsigned int i = 1; i <= writeArraySize; ++i)
                        {
                                testArray2->pushBack(5*i*(id+1));
                        }

                        writeStartVector.push_back(id*writeArraySize);
                        writeStrideVector.push_back(1);
                        writeCountVector.push_back(writeArraySize);
                        writeDataSizeVector.push_back(writeArraySize*(int)((size-numServersCores) / 2));
                        writeController = XdmfHDF5ControllerDSM::New(
                                newPath,
                                newSetPath,
                                XdmfArrayType::Int32(),
                                writeStartVector,
                                writeStrideVector,
                                writeCountVector,
                                writeDataSizeVector,
                                exampleWriter->getServerBuffer());

                        testArray->insert(writeController);

                        writeController2 = XdmfHDF5ControllerDSM::New(
                                newPath,
                                secondSetPath,
                                XdmfArrayType::Int32(),
                                writeStartVector,
                                writeStrideVector,
                                writeCountVector,
                                writeDataSizeVector,
                                exampleWriter->getServerBuffer());

                        testArray2->insert(writeController2);

                        readStartVector.push_back(0);
                        readStrideVector.push_back(1);
                        readCountVector.push_back(0);
                        readDataSizeVector.push_back(writeArraySize*(int)((size-numServersCores) / 2));

                        readController = XdmfHDF5ControllerDSM::New(
                                newPath,
                                newSetPath,
                                XdmfArrayType::Int32(),
                                readStartVector,
                                readStrideVector,
                                readCountVector,
                                readDataSizeVector,
                                exampleWriter->getServerBuffer());
                        readArray->insert(readController);

                        for (unsigned int i = 0; i<size-numServersCores; ++i)
                        {
                                MPI_Barrier(writeComm);
                                if (i == (unsigned int)id)
                                {
                                        for(unsigned int i=0; i<testArray->getSize(); ++i)
                                        {
                                                std::cout << "core #" << id <<" testArray[" << i << "] = " << testArray->getValue<int>(i) << std::endl;
                                        }
                                }
                        }

                        for (unsigned int i = 0; i<size-numServersCores; ++i)
                        {
                                MPI_Barrier(writeComm);
                                if (i == (unsigned int)id)
                                {
                                        for(unsigned int i=0; i<testArray2->getSize(); ++i)
                                        {
                                                std::cout << "core #" << id <<" testArray2[" << i << "] = " << testArray2->getValue<int>(i) << std::endl;
                                        }
                                }
                        }
                }
                else
                {
                        // Reader is second
                        readStartVector.push_back(0);
                        readStrideVector.push_back(1);
                        readCountVector.push_back(writeArraySize*(int)((size-numServersCores) / 2));
                        readDataSizeVector.push_back(writeArraySize*(int)((size-numServersCores) / 2));

                        readController = XdmfHDF5ControllerDSM::New(
                                newPath,
                                newSetPath,
                                XdmfArrayType::Int32(),
                                readStartVector,
                                readStrideVector,
                                readCountVector,
                                readDataSizeVector,
                                exampleWriter->getServerBuffer());

                        readArray->insert(readController);

                        readOutputCountVector.push_back(0);

                        readOutputController = XdmfHDF5ControllerDSM::New(
                                newPath,
                                newSetPath,
                                XdmfArrayType::Int32(),
                                readStartVector,
                                readStrideVector,
                                readOutputCountVector,
                                readDataSizeVector,
                                exampleWriter->getServerBuffer());

                        writeStartVector.push_back(0);
                        writeStrideVector.push_back(1);
                        writeCountVector.push_back(0);
                        writeDataSizeVector.push_back(writeArraySize*(int)((size-numServersCores) / 2));
                        writeController = XdmfHDF5ControllerDSM::New(
                                newPath,
                                newSetPath,
                                XdmfArrayType::Int32(),
                                writeStartVector,
                                writeStrideVector,
                                writeCountVector,
                                writeDataSizeVector,
                                exampleWriter->getServerBuffer());

                        writeController2 = XdmfHDF5ControllerDSM::New(
                                newPath,
                                secondSetPath,
                                XdmfArrayType::Int32(),
                                writeStartVector,
                                writeStrideVector,
                                writeCountVector,
                                writeDataSizeVector,
                                exampleWriter->getServerBuffer());

                        testArray2->insert(writeController2);

                        testArray->insert(writeController);
                }

                testArray->accept(exampleWriter);
                MPI_Barrier(workerComm);

//                exampleWriter->openFile();
///*
                std::cout << "getting dataset size" << std::endl;
                int checkedsize = exampleWriter->getDataSetSize(newPath, newSetPath);
                std::cout << "polled size " << checkedsize << " from file should be " << writeArraySize*(int)((size-numServersCores) / 2) << std::endl;
                assert(checkedsize == writeArraySize*(int)((size-numServersCores) / 2));
                checkedsize = exampleWriter->getDataSetSize(newPath, "empty");
                std::cout << "polled nonexistant set size =  " << checkedsize << std::endl;
                assert(checkedsize == 0);
//*/
                MPI_Barrier(workerComm);
//                exampleWriter->closeFile();

                testArray2->accept(exampleWriter);

                MPI_Barrier(workerComm);

                // This is the loop that manipulates the data
                for (unsigned int iteration = 0; iteration < 10; ++iteration)
                {
                        std::cout << "loop iteration " << iteration << " on core " << id << std::endl;

                        // Read in to the first set of cores
                        if (id >= (int)((size - numServersCores) / 2))
                        {
                                // Controllers are accessed like this since the writer removes them and creates its own.
                                shared_dynamic_cast<XdmfHDF5ControllerDSM>(readArray->getHeavyDataController(0))->setWorkerComm(readComm);

                                readArray->read();

                                MPI_Barrier(readComm);

                                if (id == (int)((size - numServersCores) / 2))
                                {
                                        for(unsigned int i=0; i<readArray->getSize(); ++i)
                                        {
                                                int tempVal = readArray->getValue<int>(i);
                                                tempVal = tempVal * 2;
                                                readArray->insert(i, tempVal);
                                                // Pull the value from the array in order to ensure the change has happened
                                                std::cout << "core #" << id <<" readArray[" << i << "] = " << readArray->getValue<int>(i) << std::endl;
                                        }
                                }
                        }
                        shared_dynamic_cast<XdmfHDF5ControllerDSM>(readArray->getHeavyDataController(0))->setWorkerComm(workerComm);

                        if (id > (int)((size - numServersCores) / 2))
                        {
                                // Only the first read core should write out data 
                                readArray->removeHeavyDataController(0);
                                readArray->insert(readOutputController);
                        }

                        readArray->accept(exampleWriter);

                        if (id > (int)((size - numServersCores) / 2))
                        {
                                // Only the first read core should write out data
                                readArray->removeHeavyDataController(0);
                                readArray->insert(readController);
                        }

                        MPI_Barrier(workerComm);

                        std::cout << "on writing cores" << std::endl;

                        if (id < (int)((size - numServersCores) / 2))
                        {
                                shared_dynamic_cast<XdmfHDF5ControllerDSM>(testArray->getHeavyDataController(0))->setWorkerComm(writeComm);
                                testArray->read();
                                MPI_Barrier(writeComm);
                                for (unsigned int i = 0; i<(unsigned int)size; ++i)
                                {
                                        MPI_Barrier(writeComm);
                                        if (i == (unsigned int)id)
                                        {
                                                for(unsigned int i=0; i<testArray->getSize(); ++i)
                                                {
                                                        int tempVal = testArray->getValue<int>(i);
                                                        unsigned int multiplier1 = std::pow(2, iteration+1);
                                                        unsigned int multiplier2 = std::pow(3, iteration);
                                                        assert(tempVal == (i+1)*(id+1) * multiplier1 * multiplier2);
                                                        tempVal = tempVal * 3;
                                                        testArray->insert(i, tempVal);
                                                        // Pull the value from the array in order to ensure the change has happened
                                                        std::cout << "core #" << id <<" testArray[" << i << "] = " << testArray->getValue<int>(i) << std::endl;
                                                }
                                        }
                                }
                                testArray2->read();
                                for (unsigned int i = 1; i <= writeArraySize; ++i)
                                {
                                        unsigned int tempVal = testArray2->getValue<unsigned int>(i - 1);
                                        unsigned int multiplier = std::pow(5, iteration);
                                        assert(5*i*(id+1) * multiplier == tempVal);
                                        testArray2->insert(i - 1, tempVal * 5);
                                        std::cout << "core #" << id <<" testArray2[" << i-1 << "] = " << testArray2->getValue<int>(i-1) << std::endl;
                                }
                        }
                        writeController->setWorkerComm(workerComm);
                        testArray->accept(exampleWriter);
                        testArray2->accept(exampleWriter);
                }

        }

        if (id == 0)
        {
                exampleWriter->stopDSM();
        }

        MPI_Barrier(comm);

        MPI_Finalize();

        return 0;
}
