#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include <vector>
#include <map>

/** @file geometry.h
 * @brief C++ bindings for geometry modeling. */

/** @brief A 3D indexed triangle mesh class.
 *
 * Attributes:
 * - vertices:  a list of vertices, given as a flattened coordinate list
 *   [x1, y1, z1, x2, y2, ...]
 * - indices: a list of triangle vertices given as indices into the
 *   vertices list, i.e., [a1,b1,c2, a2,b2,c2, ...]
 *
 * Note: because the bindings are generated by SWIG, you can access
 * the indices / vertices members via some automatically generated
 * accessors / modifiers.  In particular len(), append(), and
 * indexing via [] are useful. Some other methods like resize() are
 * also provided.  However, you CANNOT set these items via assignment. 
 *
 * Examples:
 *
 * m = TriangleMesh()
 * m.vertices.append(0)
 * m.vertices.append(0)
 * m.vertices.append(0)
 * print len(m.vertices)  #prints 3
 * m.vertices = [0,0,0]   #this is an error
 * m.vertices += [1,2,3]   #this is also an error
 */
struct TriangleMesh
{
  ///Translates all the vertices by v=v+t
  void translate(const double t[3]);
  ///Transforms all the vertices by the rigid transform v=R*v+t
  void transform(const double R[9],const double t[3]);

  std::vector<int> indices;
  std::vector<double> vertices;
};

/** @brief A 3D point cloud class.  
 *
 * Attributes:
 * - vertices: a list of vertices, given as a list [x1, y1, z1, x2, y2, ... zn]
 * - properties: a list of vertex properties, given as a list
 * [p11, p21, ..., pk1,  p12, p22, ..., pk2, ... , pn1, pn2, ..., pn2]
 * where each vertex has k properties.  The name of each property is given
 * by the propertyNames member.
 *
 * Note: because the bindings are generated by SWIG, you can access
 * the vertices/properties/propertyName members via some automatically
 * generated accessors / modifiers.  In particular len(), append(), and
 * indexing via [] are useful. Some other methods like resize() are
 * also provided.  However, you CANNOT set these items via assignment. 
 *
 * Examples:
 *
 * pc = PointCloud()
 * pc.propertyNames.append('rgb')
 * pc.vertices.append(0)
 * pc.vertices.append(0)
 * pc.vertices.append(0)
 * pc.properties.append(0)
 * print len(pc.vertices)  #prints 3
 * print pc.numPoints()  #prints 1
 * pc.addPoint([1,2,3])
 * print pc.numPoints()  #prints 2
 * print len(pc.properties.size()) #prints 2: 1 property category x 2 points
 * print pc.getProperty(1,0) #prints 0; this is the default value added when addPoint is called
 */
struct PointCloud
{
  ///Returns the number of points
  int numPoints() const;
  ///Returns the number of properties
  int numProperties() const;
  ///Sets all the points to the given list (a 3n-list)
  void setPoints(int num,const std::vector<double>& plist);
  ///Adds a point. Sets all its properties to 0.  Returns the index.
  int addPoint(const double p[3]);
  ///Sets the position of the point at the given index to p
  void setPoint(int index,const double p[3]);
  ///Retrieves the position of the point at the given index
  void getPoint(int index,double out[3]) const;
  ///Adds a new property.  All values for this property are set to 0.
  void addProperty(const std::string& pname);
  ///Adds a new property with name pname, and sets values for this property to the given list (a n-list)
  void addProperty(const std::string& pname,const std::vector<double> & properties);
  ///Sets all the properties of all points to the given list (a kn-list)
  void setProperties(const std::vector<double>& properties);
  ///Sets property pindex of all points to the given list (a n-list)
  void setProperties(int pindex,const std::vector<double>& properties);
  ///Sets property pindex of point index to the given value
  void setProperty(int index,int pindex,double value);
  ///Sets the property named pname of point index to the given value
  void setProperty(int index,const std::string& pname,double value);
  ///Gets property pindex of point index 
  double getProperty(int index,int pindex) const;
  ///Gets the property named pname of point index
  double getProperty(int index,const std::string& pname) const;
  ///Translates all the points by v=v+t
  void translate(const double t[3]);
  ///Transforms all the points by the rigid transform v=R*v+t
  void transform(const double R[9],const double t[3]);
  ///Adds the given point cloud to this one.  They must share the same
  ///properties or else an exception is raised
  void join(const PointCloud& pc);
  ///Sets the given setting
  void setSetting(const std::string& key,const std::string& value);
  ///Retrieves the given setting
  std::string getSetting(const std::string& key) const;

  std::vector<double> vertices;
  std::vector<std::string> propertyNames;
  std::vector<double> properties;
  std::map<std::string,std::string> settings;
};

/** @brief A geometric primitive.  So far only points, spheres, segments,
 * and AABBs can be constructed manually in the Python API. 
 */
struct GeometricPrimitive
{
  void setPoint(const double pt[3]);
  void setSphere(const double c[3],double r);
  void setSegment(const double a[3],const double b[3]);
  void setAABB(const double bmin[3],const double bmax[3]);
  bool loadString(const char* str);
  std::string saveString() const;

  std::string type;
  std::vector<double> properties;
};

/** @brief A three-D geometry.  Can either be a reference to a
 * world item's geometry, in which case modifiers change the 
 * world item's geometry, or it can be a standalone geometry.
 *
 * There are four currently supported types of geometry:
 * - primitives (GeometricPrimitive)
 * - triangle meshes (TriangleMesh)
 * - point clouds (PointCloud)
 * - groups (Group)
 * This class acts as a uniform container of all of these types.
 *
 * Each geometry stores a "current" transform, which is automatically updated
 * for world items' geometries.  The proximity queries are performed with 
 * respect to the transformed geometries (note the underlying geometry is 
 * not changed, which could be computationally expensive.  The query is
 * performed, however, as though they were).
 *
 * If you want to set a world item's geometry to be equal to a standalone
 * geometry, use the set(rhs) function rather than the assignment (=)
 * operator.
 *
 * Modifiers include any setX() functions, translate(), and transform().
 *
 * Proximity queries include collides(), withinDistance(), distance(), 
 * closestPoint(), and rayCast().
 *
 * Each object also has a "collision margin" which may virtually fatten the
 * object, as far as proximity queries are concerned. This is useful
 * for setting collision avoidance margins in motion planning. By
 * default it is zero.  (Note that this is NOT the same thing as simulation
 * body collision padding!)
 */
class Geometry3D
{
 public:
  Geometry3D();
  Geometry3D(const Geometry3D&);
  Geometry3D(const GeometricPrimitive&);
  Geometry3D(const TriangleMesh&);
  Geometry3D(const PointCloud&);
  ~Geometry3D();
  const Geometry3D& operator = (const Geometry3D& rhs);
  ///Creates a standalone geometry from this geometry
  Geometry3D clone();
  ///Copies the geometry of the argument into this geometry.
  void set(const Geometry3D&);
  ///Returns true if this is a standalone geometry
  bool isStandalone();
  ///Frees the data associated with this geometry, if standalone 
  void free();
  ///Returns the type of geometry: TriangleMesh, PointCloud, or
  ///GeometricPrimitive
  std::string type();
  ///Returns true if this has no contents (not the same as numElements()==0)
  bool empty();
  ///Returns a TriangleMesh if this geometry is of type TriangleMesh
  TriangleMesh getTriangleMesh();
  ///Returns a PointCloud if this geometry is of type PointCloud
  PointCloud getPointCloud();
  ///Returns a GeometricPrimitive if this geometry is of type GeometricPrimitive
  GeometricPrimitive getGeometricPrimitive();
  ///Sets this Geometry3D to a TriangleMesh
  void setTriangleMesh(const TriangleMesh&);
  ///Sets this Geometry3D to a PointCloud
  void setPointCloud(const PointCloud&);
  ///Sets this Geometry3D to a GeometricPrimitive
  void setGeometricPrimitive(const GeometricPrimitive&);
  ///Sets this Geometry3D to a group geometry.  To add sub-geometries, repeatedly call
  ///setElement()
  void setGroup();
  ///Returns an element of the Geometry3D if it is a group.  Raises an error if this
  ///is of any other type.
  Geometry3D getElement(int element);
  ///Sets an element of the Geometry3D if it is a group.  Raises an error if this is
  ///of any other type.  
  void setElement(int element,const Geometry3D& data);
  ///Returns the number of sub-elements in this geometry
  int numElements();

  ///Loads from file.  Standard mesh types, PCD files, and .geom files are
  ///supported.
  bool loadFile(const char* fn);
  ///Saves to file.  Standard mesh types, PCD files, and .geom files are
  ///supported.
  bool saveFile(const char* fn);
  ///Sets the current transformation (not modifying the underlying data)
  void setCurrentTransform(const double R[9],const double t[3]);
  ///Gets the current transformation 
  void getCurrentTransform(double out[9],double out2[3]);
  ///Translates the geometry data.
  ///Permanently modifies the data and resets any collision data structures.
  void translate(const double t[3]);
  ///Scales the geometry data uniformly.
  ///Permanently modifies the data and resets any collision data structures.
  void scale(double s);
  ///Scales the geometry data with different factors on each axis.
  ///Permanently modifies the data and resets any collision data structures.
  void scale(double sx,double sy,double sz);
  ///Rotates the geometry data.
  ///Permanently modifies the data and resets any collision data structures.
  void rotate(const double R[9]);
  ///Translates/rotates/scales the geometry data.
  ///Permanently modifies the data and resets any collision data structures.
  void transform(const double R[9],const double t[3]);
  ///Sets a padding around the base geometry which affects the results of
  ///proximity queries
  void setCollisionMargin(double margin);
  ///Returns the padding around the base geometry.  Default 0
  double getCollisionMargin();
  ///Returns the axis-aligned bounding box of the object. Note: O(1) time, but may not be tight
  void getBB(double out[3],double out2[3]);
  ///Returns a tighter axis-aligned bounding box of the object than getBB. Worst case O(n) time.
  void getBBTight(double out[3],double out2[3]);
  ///Returns true if this geometry collides with the other
  bool collides(const Geometry3D& other);
  ///Returns true if this geometry is within distance tol to other
  bool withinDistance(const Geometry3D& other,double tol);
  ///Returns the distance from this geometry to the other
  double distance(const Geometry3D& other,double relErr=0,double absErr=0);
  ///Returns (success,cp) giving the closest point to the input point.
  ///success is false if that operation is not supported with the given
  ///geometry type.
  ///cp are given in world coordinates.
  bool closestPoint(const double pt[3],double out[3]);
  ///Returns (hit,pt) where hit is true if the ray starting at s and pointing
  ///in direction d hits the geometry (given in world coordinates); pt is
  ///the hit point, in world coordinates.
  bool rayCast(const double s[3],const double d[3],double out[3]);

  int world;
  int id;
  void* geomPtr;
};

#endif
