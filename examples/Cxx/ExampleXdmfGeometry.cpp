#include "XdmfGeometry.hpp"
#include "XdmfGeometryType.hpp"

int main(int, char **)
{
        //#initialization begin

        shared_ptr<XdmfGeometry> exampleGeometry = XdmfGeometry::New();

        //#initialization end

        //Assuming that exampleGeometry is a shared pointer to a XdmfGeometry object

        //#setType begin

        exampleGeometry->setType(XdmfGeometryType::XYZ());

        //#setType end

        //#getType begin

        shared_ptr<const XdmfGeometryType> exampleType = exampleGeometry->getType();

        //#getType end

        //#getNumberPoints begin

        unsigned int numPoints = exampleGeometry->getNumberPoints();

        //#getNumberPoints end

        //#setOrigin begin

        double newXval = 5.0;
        double newYval = 10.0;
        double newZval = 5.5;

        exampleGeometry->setOrigin(newXval, newYval, newZval);

        //#setOrigin end

        //#getOrigin begin

        std::vector<double> exampleGeometry->getOrigin();

        //#getOrigin end

        return 0;
}
