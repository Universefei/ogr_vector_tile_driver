/******************************************************************************
 * $Id: ogrgeojsonreader.h 27044 2014-03-16 23:41:27Z rouault $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Defines GeoJSON reader within OGR OGRGeoJSON Driver.
 * Author:   Mateusz Loskot, mateusz@loskot.net
 *
 ****************************************************************************/
#ifndef OGR_GEOJSONREADER_H_INCLUDED
#define OGR_GEOJSONREADER_H_INCLUDED

#include <ogr_core.h>
#include <json.h> // JSON-C

/************************************************************************/
/*                         FORWARD DECLARATIONS                         */
/************************************************************************/

class OGRGeometry;
class OGRRawPoint;
class OGRPoint;
class OGRMultiPoint;
class OGRLineString;
class OGRMultiLineString;
class OGRLinearRing;
class OGRPolygon;
class OGRMultiPolygon;
class OGRGeometryCollection;
class OGRFeature;
class OGRGeoJSONLayer;
class OGRSpatialReference;

/************************************************************************/
/*                           GeoJSONObject                              */
/************************************************************************/

struct GeoJSONObject
{
    enum Type
    {
        eUnknown = wkbUnknown, // non-GeoJSON properties
        ePoint = wkbPoint,
        eLineString = wkbLineString,
        ePolygon = wkbPolygon,
        eMultiPoint = wkbMultiPoint,
        eMultiLineString = wkbMultiLineString,
        eMultiPolygon = wkbMultiPolygon,
        eGeometryCollection = wkbGeometryCollection,
        eFeature,
        eFeatureCollection
    };
    
    enum CoordinateDimension
    {
        eMinCoordinateDimension = 2,
        eMaxCoordinateDimension = 3
    };
};

/************************************************************************/
/*                           OGRGeoJSONReader                           */
/************************************************************************/

class OGRGeoJSONDataSource;

class OGRGeoJSONReader
{
public:

    OGRGeoJSONReader();
    ~OGRGeoJSONReader();

    void SetPreserveGeometryType( bool bPreserve );
    void SetSkipAttributes( bool bSkip );

    OGRErr Parse( const char* pszText );
    /* void ReadLayers( OGRGeoJSONDataSource* poDS ); */
    /* void ReadLayer( OGRGeoJSONDataSource* poDS, */
    /*                 const char* pszName, */
    /*                 json_object* poObj ); */
    void ReadLayers( VectorTile* poVTile );
    void ReadLayer( VectorTile* poVTile,
                    const char* pszName,
                    json_object* poObj );

private:

    json_object* poGJObject_;

    bool bGeometryPreserve_;
    bool bAttributesSkip_;

    int bFlattenGeocouchSpatiallistFormat;
    bool bFoundId, bFoundRev, bFoundTypeFeature, bIsGeocouchSpatiallistFormat;

    //
    // Copy operations not supported.
    //
    OGRGeoJSONReader( OGRGeoJSONReader const& );
    OGRGeoJSONReader& operator=( OGRGeoJSONReader const& );

    //
    // Translation utilities.
    //
    bool GenerateLayerDefn( OGRGeoJSONLayer* poLayer, json_object* poGJObject );
    bool GenerateFeatureDefn( OGRGeoJSONLayer* poLayer, json_object* poObj );

    /* bool AddFeature( OGRGeoJSONLayer* poLayer, OGRGeometry* poGeometry ); */
    /* bool AddFeature( OGRGeoJSONLayer* poLayer, OGRFeature* poFeature ); */
    bool AddFeature( VectorTile* poTile, OGRGeometry* poGeometry );
    bool AddFeature( VectorTile* poTile, OGRFeature* poFeature );

    OGRGeometry* ReadGeometry( json_object* poObj );
    /* OGRFeature* ReadFeature( OGRGeoJSONLayer* poLayer, json_object* poObj ); */
    OGRFeature* ReadFeature( VectorTile* poLayer, json_object* poObj );

    /* void ReadFeatureCollection( OGRGeoJSONLayer* poLayer, json_object* poObj ); */
    void ReadFeatureCollection( VectorTile* poLayer, json_object* poObj );

};

/************************************************************************/
/*                 GeoJSON Parsing Utilities                            */
/************************************************************************/

json_object* OGRGeoJSONFindMemberByName(json_object* poObj,  const char* pszName );
GeoJSONObject::Type OGRGeoJSONGetType( json_object* poObj );

/************************************************************************/
/*                 GeoJSON Geometry Translators                         */
/************************************************************************/

bool OGRGeoJSONReadRawPoint( json_object* poObj, OGRPoint& point );
OGRGeometry* OGRGeoJSONReadGeometry( json_object* poObj );
OGRPoint* OGRGeoJSONReadPoint( json_object* poObj );
OGRMultiPoint* OGRGeoJSONReadMultiPoint( json_object* poObj );
OGRLineString* OGRGeoJSONReadLineString( json_object* poObj, bool bRaw=false );
OGRMultiLineString* OGRGeoJSONReadMultiLineString( json_object* poObj );
OGRLinearRing* OGRGeoJSONReadLinearRing( json_object* poObj );
OGRPolygon* OGRGeoJSONReadPolygon( json_object* poObj , bool bRaw=false);
OGRMultiPolygon* OGRGeoJSONReadMultiPolygon( json_object* poObj );
OGRGeometryCollection* OGRGeoJSONReadGeometryCollection( json_object* poObj );
OGRSpatialReference* OGRGeoJSONReadSpatialReference( json_object* poObj );

#endif /* OGR_GEOJSONUTILS_H_INCLUDED */
