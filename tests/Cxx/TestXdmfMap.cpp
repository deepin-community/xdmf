#include "XdmfAttribute.hpp"
#include "XdmfAttributeCenter.hpp"
#include "XdmfAttributeType.hpp"
#include "XdmfGeometry.hpp"
#include "XdmfGeometryType.hpp"
#include "XdmfGridCollection.hpp"
#include "XdmfGridCollectionType.hpp"
#include "XdmfHDF5Writer.hpp"
#include "XdmfMap.hpp"
#include "XdmfReader.hpp"
#include "XdmfTestCompareFiles.hpp"
#include "XdmfTopology.hpp"
#include "XdmfTopologyType.hpp"
#include "XdmfUnstructuredGrid.hpp"
#include "XdmfWriter.hpp"

#include <iostream>

void performTests(std::vector<shared_ptr<XdmfMap> > & boundaryMaps)
{
  boundaryMaps[0]->read();
  boundaryMaps[1]->read();

  // localNodeId || remoteLocalNodeId
  XdmfMap::node_id_map mapping;

  mapping = boundaryMaps[0]->getRemoteNodeIds(1);
  std::cout << mapping.size() << " ?= " << 1 << std::endl;
  std::cout << mapping[1].size() << " ?= " << 1 << std::endl;
  std::cout << *(mapping[1].begin()) << " ?= " << 0 << std::endl;
  assert(mapping.size() == 1);
  assert(mapping[1].size() == 1);
  assert(*(mapping[1].begin()) == 0);

  mapping = boundaryMaps[1]->getRemoteNodeIds(0);
  std::cout << mapping.size() << " ?= " << 1 << std::endl;
  std::cout << mapping[0].size() << " ?= " << 1 << std::endl;
  std::cout << *(mapping[0].begin()) << " ?= " << 1 << std::endl;
  assert(mapping.size() == 1);
  assert(mapping[0].size() == 1);
  assert(*(mapping[0].begin()) == 1);
}

/*
 * (local, global)
 *
 * Grid 0:
 *
 * (0, 0) - - - (1, 1)
 *
 * Grid 1:
 *
 * (0, 1) - - - (1, 2)
 *
 */

int main(int, char **)
{
  // Grid 0
  shared_ptr<XdmfUnstructuredGrid> grid0 = XdmfUnstructuredGrid::New();
  grid0->getGeometry()->setType(XdmfGeometryType::XYZ());
  double points0[] = {-1, 0, 0, 0, 0, 0};
  grid0->getGeometry()->insert(0, &points0[0], 6);
  grid0->getTopology()->setType(XdmfTopologyType::Polyline(2));
  unsigned int connectivity0[] = {0, 1};
  grid0->getTopology()->insert(0, &connectivity0[0], 2);
  shared_ptr<XdmfAttribute> globalNodeIds0 = XdmfAttribute::New();
  globalNodeIds0->setName("GlobalNodeId");
  globalNodeIds0->setCenter(XdmfAttributeCenter::Node());
  globalNodeIds0->setType(XdmfAttributeType::GlobalId());
  unsigned int globalVals0[] = {0, 1};
  globalNodeIds0->insert(0, &globalVals0[0], 2);

  // Grid 1
  shared_ptr<XdmfUnstructuredGrid> grid1 = XdmfUnstructuredGrid::New();
  grid1->getGeometry()->setType(XdmfGeometryType::XYZ());
  double points1[] = {0, 0, 0, 1, 0, 0};
  grid1->getGeometry()->insert(0, &points1[0], 6);
  grid1->getTopology()->setType(XdmfTopologyType::Polyline(2));
  unsigned int connectivity1[] = {0, 1};
  grid1->getTopology()->insert(0, &connectivity1[0], 2);
  shared_ptr<XdmfAttribute> globalNodeIds1 = XdmfAttribute::New();
  globalNodeIds1->setName("GlobalNodeId");
  globalNodeIds1->setCenter(XdmfAttributeCenter::Node());
  globalNodeIds1->setType(XdmfAttributeType::GlobalId());
  unsigned int globalVals1[] = {1, 2};
  globalNodeIds1->insert(0, &globalVals1[0], 2);

  std::vector<shared_ptr<XdmfAttribute> > globalNodeIds;
  globalNodeIds.push_back(globalNodeIds0);
  globalNodeIds.push_back(globalNodeIds1);

  std::vector<shared_ptr<XdmfMap> > boundaryMaps =
    XdmfMap::New(globalNodeIds);

  performTests(boundaryMaps);
  grid0->insert(boundaryMaps[0]);
  grid1->insert(boundaryMaps[1]);

  // Grid Collection
  shared_ptr<XdmfGridCollection> collection = XdmfGridCollection::New();
  collection->setType(XdmfGridCollectionType::Spatial());
  collection->insert(grid0);
  collection->insert(grid1);
  shared_ptr<XdmfDomain> domain = XdmfDomain::New();
  domain->insert(collection);

  shared_ptr<XdmfWriter> writer = XdmfWriter::New("TestXdmfMap1.xmf");
  domain->accept(writer);

  shared_ptr<XdmfReader> reader = XdmfReader::New();
  shared_ptr<XdmfDomain> domain2 =
    shared_dynamic_cast<XdmfDomain>(reader->read("TestXdmfMap1.xmf"));

  boundaryMaps.clear();
  boundaryMaps.push_back(domain2->getGridCollection(0)->getUnstructuredGrid(0)->getMap(0));
  boundaryMaps.push_back(domain2->getGridCollection(0)->getUnstructuredGrid(1)->getMap(0));
  performTests(boundaryMaps);

  shared_ptr<XdmfWriter> writer2 = XdmfWriter::New("TestXdmfMap2.xmf");
  domain2->accept(writer2);

  if (XdmfTestCompareFiles::compareFiles("TestXdmfMap1.xmf",
                                         "TestXdmfMap2.xmf"))
  {
    std::cout << "compared files are the same" << std::endl;
  }
  else
  {
    std::cout << "compared files are not the same" << std::endl;
  }

  assert(XdmfTestCompareFiles::compareFiles("TestXdmfMap1.xmf",
                                            "TestXdmfMap2.xmf"));

  shared_ptr<XdmfWriter> writerHDF =
    XdmfWriter::New("TestXdmfMapHDF1.xmf");
  writerHDF->setLightDataLimit(0);
  domain->accept(writerHDF);

  shared_ptr<XdmfDomain> domainHDF =
    shared_dynamic_cast<XdmfDomain>(reader->read("TestXdmfMapHDF1.xmf"));

  boundaryMaps.clear();
  boundaryMaps.push_back(domainHDF->getGridCollection(0)->getUnstructuredGrid(0)->getMap(0));
  boundaryMaps.push_back(domainHDF->getGridCollection(0)->getUnstructuredGrid(1)->getMap(0));
  performTests(boundaryMaps);

  shared_ptr<XdmfWriter> writerHDF2 =
    XdmfWriter::New("TestXdmfMapHDF2.xmf");
  writerHDF2->getHeavyDataWriter()->setMode(XdmfHeavyDataWriter::Overwrite);
  writerHDF2->setLightDataLimit(0);
  domainHDF->accept(writerHDF2);

  if (XdmfTestCompareFiles::compareFiles("TestXdmfMapHDF1.xmf",
                                         "TestXdmfMapHDF2.xmf"))
  {
    std::cout << "compared files are the same" << std::endl;
  }
  else
  {
    std::cout << "compared files are not the same" << std::endl;
  }

  assert(XdmfTestCompareFiles::compareFiles("TestXdmfMapHDF1.xmf",
                                            "TestXdmfMapHDF2.xmf"));

  return 0;
}
