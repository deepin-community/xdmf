#include "XdmfSparseMatrix.hpp"
#include "XdmfWriter.hpp"
#include "stdio.h"
#include <iostream>

int main(int, char **)
{
  shared_ptr<XdmfSparseMatrix> matrix = XdmfSparseMatrix::New(3, 3);

  matrix->setName("foo");
  shared_ptr<XdmfArray> rowPointer = matrix->getRowPointer();
  shared_ptr<XdmfArray> columnIndex = matrix->getColumnIndex();
  shared_ptr<XdmfArray> values = matrix->getValues();

  rowPointer->insert<unsigned int>(0, 0);
  rowPointer->insert<unsigned int>(1, 1);
  rowPointer->insert<unsigned int>(2, 2);
  rowPointer->insert<unsigned int>(3, 3);
  columnIndex->pushBack<unsigned int>(1);
  columnIndex->pushBack<unsigned int>(2);
  columnIndex->pushBack<unsigned int>(0);
  values->pushBack<double>(5.0);
  values->pushBack<double>(6.0);
  values->pushBack<double>(-1.0);

  std::cout << "matrix contains" << "\n" << matrix->getValuesString() << std::endl;

  assert(matrix->getValuesString().compare("0.0, 5, 0.0, \n0.0, 0.0, 6, \n-1, 0.0, 0.0, \n") == 0);

  shared_ptr<XdmfWriter> writer = XdmfWriter::New("TestXdmfSparseMatrix.xmf");
  matrix->accept(writer);

  return 0;
}
