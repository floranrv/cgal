#ifndef CGAL_IO_ADVANCING_FRONT_SURFACE_RECONSTRUCTION_H
#define CGAL_IO_ADVANCING_FRONT_SURFACE_RECONSTRUCTION_H


#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/algorithm.h>
#include <CGAL/Unique_hash_map.h>
#include <CGAL/Random.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Tvb_3_2.h>
#include <CGAL/AFSR/Surface_vertex_base_2.h>
#include <CGAL/AFSR/Surface_face_base_2.h>
#include <CGAL/AFSR/orient.h>
#include <CGAL/IO/AFSR_vrml.h>

namespace CGAL {


template <class Vertex>
struct Is_not_exterior {
  bool operator()(const Vertex& v)const {
    return ! v.is_exterior();
  }
};

template <class Surface>
void
write_to_file_medit(char* foutput, const Surface& S)
{
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Vertex Vertex;
  typedef typename Surface::Cell_handle Cell_handle;

  Triangulation_3& T = S.triangulation();

  char foutput_points[100];
  char foutput_faces[100];
  std::strcpy(foutput_points, foutput);
  std::strcpy(foutput_faces, foutput);
  strcat(foutput_points, ".points");
  strcat(foutput_faces, ".faces");
  std::ofstream os_points(foutput_points, std::ios::out);
  std::ofstream os_faces(foutput_faces, std::ios::out);
  if((os_points.fail())||(os_faces.fail()))
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> files for output : " << foutput_points 
	      << ", " << foutput_faces << std::endl;

  os_points.clear();
  os_faces.clear();

  CGAL::set_ascii_mode(os_points);
  CGAL::set_ascii_mode(os_faces);

  // af: what is the relationship to _vh_number in Extract_surface
  int _vh_number = std::count_if(T.finite_vertices_begin(), 
				 T.finite_vertices_end(), 
				 Is_not_exterior<Vertex>());

  os_points << _vh_number << std::endl;

  CGAL::Unique_hash_map<Vertex_handle, int> vertex_index_map(-1, T.number_of_vertices());

  int count(0);
  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++){
    typename CGAL::Unique_hash_map<Vertex_handle, int>::Data& d = vertex_index_map[v_it];
    if ((!v_it->is_exterior()) && d == -1){
      d = count;
      count++;
      os_points << v_it->point() << " 0" << std::endl;
    }
  }
  
  os_faces << S.number_of_facets() << std::endl;
  for(Finite_facets_iterator f_it = T.finite_facets_begin(); 
      f_it != T.finite_facets_end(); 
      f_it++)
    {
      Cell_handle n, c = (*f_it).first;
      int ni, ci = (*f_it).second;
      n = c->neighbor(ci);
      ni = n->index(c);
      int i1, i2 ,i3;

      if (c->is_selected_facet(ci))
	{
	  i1 = (ci+1) & 3;
	  i2 = (ci+2) & 3;
	  i3 = (ci+3) & 3;
	  os_faces << 3 << " ";
	  os_faces << vertex_index_map[c->vertex(i1)] + 1 << " ";
	  os_faces << vertex_index_map[c->vertex(i2)] + 1 << " ";
	  os_faces << vertex_index_map[c->vertex(i3)] + 1 << " ";
	  os_faces << " 0 0 0 0" << std::endl; 
	}
      
       if (n->is_selected_facet(ni))
	{
	  i1 = (ni+1) & 3;
	  i2 = (ni+2) & 3;
	  i3 = (ni+3) & 3;
	  os_faces << 3 << " ";
	  os_faces << vertex_index_map[n->vertex(i1)] + 1 << " ";
	  os_faces << vertex_index_map[n->vertex(i2)] + 1 << " ";
	  os_faces << vertex_index_map[n->vertex(i3)] + 1 << " ";
	  os_faces << " 0 0 0 0" << std::endl; 
	}
    }

  std::cout << "-- medit result written." << std::endl;
}

//---------------------------------------------------------------------

template <class Surface>
void
write_to_file_gv(char* foutput, const Surface& S)
{ 
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Vertex Vertex;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();

  char foutput_tmp[100];
  std::strcpy(foutput_tmp, foutput);

  strcat(foutput_tmp, ".off");
  std::ofstream os(foutput_tmp, std::ios::out);

  if(os.fail())
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> file for output : " << foutput_tmp << std::endl;

  os.clear();

  CGAL::set_ascii_mode(os);

  int _vh_number = std::count_if(T.finite_vertices_begin(), 
			     T.finite_vertices_end(), 
			     Is_not_exterior<Vertex>());
  // Header.
  os << "OFF" << std::endl
     << _vh_number << " " << S.number_of_facets() << " " << 0 << std::endl;

  CGAL::Unique_hash_map<Vertex_handle, int> vertex_index_map(-1, T.number_of_vertices());

  int count(0);
  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++){
    typename CGAL::Unique_hash_map<Vertex_handle, int>::Data& d = vertex_index_map[v_it];
    if ((!v_it->is_exterior()) && d == -1){
      d = count;
      count++;
      os << v_it->point()  << " \n";
    }
  }
     
  for(Finite_facets_iterator f_it = T.finite_facets_begin(); 
      f_it != T.finite_facets_end(); 
      f_it++)
    {
      Cell_handle n, c = (*f_it).first;
      int ni, ci = (*f_it).second;
      n = c->neighbor(ci);
      ni = n->index(c);
      int i1, i2 ,i3;

      if (c->is_selected_facet(ci))
	{
	  i1 = (ci+1) & 3;
	  i2 = (ci+2) & 3;
	  i3 = (ci+3) & 3;
	  os << 3 << " ";
	  os << vertex_index_map[c->vertex(i1)] << " ";
	  os << vertex_index_map[c->vertex(i2)] << " ";
	  os << vertex_index_map[c->vertex(i3)] << " ";
	  os << 0 << std::endl; // without color.
	  // os << 4 << drand48() << drand48() << drand48() << 1.0; // random
	  // color
	}

       if (n->is_selected_facet(ni))
	{
	  i1 = (ni+1) & 3;
	  i2 = (ni+2) & 3;
	  i3 = (ni+3) & 3;
	  os << 3 << " ";
	  os << vertex_index_map[n->vertex(i1)] << " ";
	  os << vertex_index_map[n->vertex(i2)] << " ";
	  os << vertex_index_map[n->vertex(i3)] << " ";
	  os << 0 << std::endl; // without color.
	  // os << 4 << drand48() << drand48() << drand48() << 1.0; // random
	  // color 
	}
    }

  std::cout << "-- oogl result written." << std::endl;
}

//---------------------------------------------------------------------
template <class Surface>
void
write_to_file_ply(char* foutput, const Surface& S)
{ 
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();
  char foutput_tmp[100];
  std::strcpy(foutput_tmp, foutput);

  strcat(foutput_tmp, ".ply");
  std::ofstream os(foutput_tmp, std::ios::out | std::ios::binary);

  if(os.fail())
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> file for output : " << foutput_tmp << std::endl;

  os.clear();

  CGAL::set_ascii_mode(os);

  // Header.
  os << "ply" << std::endl
     << "format binary_little_endian 1.0" << std::endl
     << "comment generated by ply_writer" << std::endl
     << "element vertex " << S.number_of_vertices() << std::endl
     << "property float x" << std::endl
     << "property float y" << std::endl
     << "property float z" << std::endl
     << "element face " << S.number_of_facets() << std::endl
     << "property list uchar int vertex_indices" << std::endl
     << "end_header" << std::endl;

  CGAL::set_binary_mode(os);

  CGAL::Unique_hash_map<Vertex_handle, int> vertex_index_map(-1, T.number_of_vertices());

  int count(0);
  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++){
    typename CGAL::Unique_hash_map<Vertex_handle, int>::Data& d = vertex_index_map[v_it];
    if ((!v_it->is_exterior()) && d == -1){
      d = count;
      count++;
      os << v_it->point()  << std::endl;
    }
  }
     
  for(Finite_facets_iterator f_it = T.finite_facets_begin(); 
      f_it != T.finite_facets_end(); 
      f_it++)
    {
      Cell_handle n, c = (*f_it).first;
      int ni, ci = (*f_it).second;
      n = c->neighbor(ci);
      ni = n->index(c);
      int i1, i2 ,i3;

      if (c->is_selected_facet(ci))
	{
	  i1 = (ci+1) & 3;
	  i2 = (ci+2) & 3;
	  i3 = (ci+3) & 3;
	  char three = '3'; 
	  CGAL::write(os, three, CGAL::io_Read_write()); 
	  CGAL::write(os,vertex_index_map[c->vertex(i1)], CGAL::io_Read_write());
	  CGAL::write(os,vertex_index_map[c->vertex(i2)], CGAL::io_Read_write());
	  CGAL::write(os,vertex_index_map[c->vertex(i3)], CGAL::io_Read_write());
	  os << std::endl; // without color.
	  // os << 4 << drand48() << drand48() << drand48() << 1.0; // random
	  // color
	}

       if (n->is_selected_facet(ni))
	{
	  i1 = (ni+1) & 3;
	  i2 = (ni+2) & 3;
	  i3 = (ni+3) & 3;
	  char three = '3'; 
	  CGAL::write(os, three, CGAL::io_Read_write()); 
	  CGAL::write(os,vertex_index_map[n->vertex(i1)], CGAL::io_Read_write());
	  CGAL::write(os,vertex_index_map[n->vertex(i2)], CGAL::io_Read_write());
	  CGAL::write(os,vertex_index_map[n->vertex(i3)], CGAL::io_Read_write());
	  os << std::endl; // without color.
	  // os << 4 << drand48() << drand48() << drand48() << 1.0; // random
	  // color 
	}
    }

  //std::cout << "-- ply result written." << std::endl;
}

//---------------------------------------------------------------------
template <class Surface>
void 
write_to_file_iv_border_edges(const Surface& S, std::ofstream& os)
{
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_edges_iterator Finite_edges_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  typedef typename Surface::Edge_like Edge_like;
  typedef typename Surface::Border_elt Border_elt;

  Triangulation_3& T = S.triangulation();
  typedef std::pair<Vertex_handle, int>  indiced_vh;
  std::map<Vertex_handle, int> _vh_vect;
  int _vh_bord_count = 0;

  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++)
    if (v_it->is_on_border())
      {
	_vh_vect.insert(indiced_vh (v_it, _vh_bord_count));
	_vh_bord_count++;
      }

  typedef const Point*  Const_point_star;
  std::vector<Const_point_star>  points_tab(_vh_bord_count);
  for (typename std::map<Vertex_handle, int>::iterator vh_it = _vh_vect.begin();
       vh_it != _vh_vect.end(); vh_it++)
    points_tab[vh_it->second] = &vh_it->first->point();
  
  os << "  Separator {" << std::endl <<
"        Switch {" << std::endl  <<
"          whichChild 0" << std::endl <<
"          Separator {" << std::endl <<
"	     BaseColor {" << std::endl <<
"	                rgb 1 0 0" << std::endl <<
"        	       }" << std::endl << 
"              Coordinate3 {" << std::endl << 
"		 point [ ";
  bool first(true);	
  for(int vh_i=0; vh_i<_vh_bord_count; vh_i++)
    {
      if (!first) os << "," << std::endl <<
"		         ";
      else
	first=false;
      os << *points_tab[vh_i];
    }
  os << " ]" << std::endl <<
"		}" << std::endl <<
"		IndexedLineSet {" << std::endl <<
"		  coordIndex [ ";

  first=true;
  for(Finite_edges_iterator e_it=T.finite_edges_begin();
      e_it!=T.finite_edges_end();
      e_it++)
    {
      Cell_handle c = (*e_it).first;
      int i1 = (*e_it).second, i2 = (*e_it).third;
      Edge_like key(c->vertex(i1), c->vertex(i2));
      Border_elt result;

      if (S.is_border_elt(key, result))
	{
	  if (!first) 
	    os << "," << std::endl << "	 	               ";
	  else
	    first=false;
	  os << _vh_vect.find(c->vertex(i1))->second << ", ";
	  os << _vh_vect.find(c->vertex(i2))->second << ", ";
	  os << -1; 
	}
    }
  os << " ]" << std::endl <<
"      }}" << std::endl <<
"    }}" << std::endl;
}

//---------------------------------------------------------------------
template <class Surface>
void 
write_to_file_iv_remaining_points(const Surface& S, std::ofstream& os)
{
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();
  typedef std::pair<Vertex_handle, int>  indiced_vh;
  std::map<Vertex_handle, int> _vh_vect;
  int _vh_bord_count(0);

  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++)
    if (v_it->is_exterior())
      {
	_vh_vect.insert(indiced_vh (v_it, _vh_bord_count));
	_vh_bord_count++;
      }

  typedef const Point*  Const_point_star;
  std::vector<Const_point_star>  points_tab(_vh_bord_count);
  for (typename std::map<Vertex_handle, int>::iterator vh_it = _vh_vect.begin();
       vh_it != _vh_vect.end(); vh_it++)
    points_tab[vh_it->second] = &vh_it->first->point();
  
  os << "  Separator {" << std::endl <<
"        Switch {" << std::endl  <<
"          whichChild 0" << std::endl <<
"          Separator {" << std::endl <<
"	     BaseColor {" << std::endl <<
"	                rgb 0 0 1" << std::endl <<
"        	       }" << std::endl << 
"              Coordinate3 {" << std::endl << 
"		 point [ ";
  bool first(true);	
  for(int vh_i=0; vh_i<_vh_bord_count; vh_i++)
    {
      if (!first) os << "," << std::endl <<
"		         ";
      else
	first=false;
      os << *points_tab[vh_i];
    }
  os << " ]" << std::endl <<
"		}" << std::endl <<
"		PointSet {" << std::endl <<
"		  startIndex  0" << std::endl <<
"		  numPoints  -1" << std::endl <<
"		}";

  os << " }" << std::endl <<
"      }}" << std::endl;
  
}

//---------------------------------------------------------------------
// attention cette procedure produit un fichier tres sale... trop de sommets...

// !!!!  bizarre : ca a l'air de buggue pour hand.xyz (seg fault...)
template <class Surface>
void 
write_to_file_iv_border_facets(const Surface& S, std::ofstream& os)
{
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  typedef typename Surface::Edge_like Edge_like;
  typedef typename Surface::Border_elt Border_elt;

  Triangulation_3& T = S.triangulation();
  typedef std::pair<Vertex_handle, int>  indiced_vh;
  std::map<Vertex_handle, int> _vh_vect;
  int _vh_bord_count(0);

  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++)
//     if (v_it->number_of_incident_border() > 0)
      {
	_vh_vect.insert(indiced_vh (v_it, _vh_bord_count));
	_vh_bord_count++;
      }

  typedef const Point*  Const_point_star;
  std::vector<Const_point_star>  points_tab(_vh_bord_count);
  for (typename std::map<Vertex_handle, int>::iterator vh_it = _vh_vect.begin();
       vh_it != _vh_vect.end(); vh_it++)
    points_tab[vh_it->second] = &vh_it->first->point();
  
  os << "  Separator {" << std::endl <<
"        Switch {" << std::endl  <<
"          whichChild 0" << std::endl <<
"          Separator {" << std::endl <<
"            ShapeHints {" << std::endl <<
"             vertexOrdering  CLOCKWISE" << std::endl <<
"             shapeType       UNKNOWN_SHAPE_TYPE" << std::endl <<
"             faceType        CONVEX" << std::endl <<
"             creaseAngle     1.0" << std::endl <<
"                        }" << std::endl <<
"	     BaseColor {" << std::endl <<
"	                rgb 0 0 1" << std::endl <<
"        	       }" << std::endl << 
"              Coordinate3 {" << std::endl << 
"		 point [ ";
  bool first(true);	
  for(int vh_i=0; vh_i<_vh_bord_count; vh_i++)
    {
      if (!first) os << "," << std::endl <<
"		         ";
      else
	first=false;
      os << *points_tab[vh_i];
    }
  os << " ]" << std::endl <<
"		}" << std::endl <<
"		IndexedFaceSet {" << std::endl <<
"		  coordIndex [ ";

  first=true;
  for(Finite_facets_iterator f_it=T.finite_facets_begin();
      f_it!=T.finite_facets_end();
      f_it++)
    {
      Cell_handle c = (*f_it).first;
      int index = (*f_it).second;
      int i1 = (index+1) & 3;
      int i2 = (index+2) & 3;
      int i3 = (index+3) & 3;
      Edge_like key12(c->vertex(i1), c->vertex(i2));
      Edge_like key13(c->vertex(i1), c->vertex(i3));
      Edge_like key32(c->vertex(i3), c->vertex(i2));
      Border_elt result;

      // les trois aretes sur le bord...
//       if (is_border_elt(key12, result)&&
// 	  is_border_elt(key13, result)&&
// 	  is_border_elt(key32, result))

      // au moins 2 aretes sur le bord...
//       if (((is_border_elt(key12, result)&&
// 	    is_border_elt(key13, result)))||
// 	  ((is_border_elt(key32, result)&&
// 	    is_border_elt(key13, result)))||
// 	  ((is_border_elt(key12, result)&&
// 	    is_border_elt(key32, result))))

      // une arete sur le bord...
      if ((is_border_elt(key12, result)||
	   is_border_elt(key13, result)||
	   is_border_elt(key32, result))&&
	  (c->is_selected_facet(index)||
	   c->neighbor(index)->is_selected_facet(c->neighbor(index)->index(c))))

      // au moins 2 aretes sur le bord...
//       if (((is_border_elt(key12, result)&&
// 	    is_border_elt(key13, result)))||
// 	  ((is_border_elt(key32, result)&&
// 	    is_border_elt(key13, result)))||
// 	  ((is_border_elt(key12, result)&&
// 	    is_border_elt(key32, result))))
	{
	  if (!first) 
	    os << "," << std::endl << "	 	               ";
	  else
	    first=false;
	  os << _vh_vect.find(c->vertex(i1))->second << ", ";
	  os << _vh_vect.find(c->vertex(i2))->second << ", ";
	  os << _vh_vect.find(c->vertex(i3))->second << ", ";

	  os << -1; 
	}
    }
  os << " ]" << std::endl <<
"      }}" << std::endl <<
"    }}" << std::endl;	
}

//---------------------------------------------------------------------
template <class Surface>
void
write_to_file_iv(char* foutput, const Surface& S,
				  const bool& boundary)
{ 
  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();
  char foutput_tmp[100];
  std::strcpy(foutput_tmp, foutput);

  strcat(foutput_tmp, ".iv");
  std::ofstream os(foutput_tmp, std::ios::out);

  if(os.fail())
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> file for output : " << foutput_tmp << std::endl;

  os.clear();

  CGAL::set_ascii_mode(os);

  // Header.
  os << 
"#Inventor V2.1 ascii" << std::endl <<
"Separator {" << std::endl  <<
"  PerspectiveCamera {" << std::endl <<
"    position 0 0 2.41421" << std::endl <<
"    nearDistance 1.41421" << std::endl <<
"    farDistance 3.41421" << std::endl <<
"    focalDistance 2.41421" << std::endl <<
"  }" << std::endl <<
"  Group {" << std::endl <<
"    Rotation {" << std::endl <<
"             }" << std::endl <<
"    DirectionalLight {" << std::endl <<
"      direction 0.2 -0.2 -0.979796" << std::endl <<
"	 }" << std::endl <<
"    ResetTransform {" << std::endl <<
"	 }  }" << std::endl <<
"  Separator {" << std::endl <<
"    Switch {" << std::endl <<
"      whichChild 0" << std::endl <<
"        Separator {" << std::endl <<
"          ShapeHints {" << std::endl <<
"             vertexOrdering  CLOCKWISE" << std::endl <<
"             shapeType       UNKNOWN_SHAPE_TYPE" << std::endl <<
"             faceType        CONVEX" << std::endl <<
"             creaseAngle     1.0" << std::endl <<
"                      }" << std::endl <<
"	    BaseColor {" << std::endl <<
"	      rgb 0.6 0.6 0.48" << std::endl <<
"        	      }" << std::endl <<
"	    Coordinate3 {" << std::endl <<
"	       point [ ";

  CGAL::Unique_hash_map<Vertex_handle, int> vertex_index_map(-1, T.number_of_vertices());

  int count(0);
  for (Finite_vertices_iterator v_it = T.finite_vertices_begin();
       v_it != T.finite_vertices_end();
       v_it++){
    typename CGAL::Unique_hash_map<Vertex_handle, int>::Data& d = vertex_index_map[v_it];
    if ((!v_it->is_exterior()) && d == -1){
      d = count;
      count++;
      os << v_it->point()  << " ,\n";
    }
  }
  os << " ]" << std::endl <<
"		         }" << std::endl <<
"	     IndexedFaceSet {" << std::endl <<
"	       coordIndex [ ";

  for(Finite_facets_iterator f_it = T.finite_facets_begin(); 
      f_it != T.finite_facets_end(); 
      f_it++)
    {
      Cell_handle n, c = (*f_it).first;
      int ni, ci = (*f_it).second;
      n = c->neighbor(ci);
      ni = n->index(c);
      int i1, i2 ,i3;

      if (c->is_selected_facet(ci))
	{

	  i1 = (ci+1) & 3;
	  i2 = (ci+2) & 3;
	  i3 = (ci+3) & 3;
	  os <<  vertex_index_map[c->vertex(i1)] << ", ";
	  os <<  vertex_index_map[c->vertex(i2)] << ", ";
	  os <<  vertex_index_map[c->vertex(i3)] << ", ";
	  os << -1;
	}

       if (n->is_selected_facet(ni))
	{
	  i1 = (ni+1) & 3;
	  i2 = (ni+2) & 3;
	  i3 = (ni+3) & 3;
	  os << vertex_index_map[n->vertex(i1)] << ", ";
	  os << vertex_index_map[n->vertex(i2)] << ", ";
	  os << vertex_index_map[n->vertex(i3)] << ", ";
	  os << -1; 
	}
    }


  os << " ]\n"
    "      }\n"
    "    }}\n"
    "  }\n";

  if (boundary)
    {
      // pour visualiser les boundaries restant a la fin...
      write_to_file_iv_border_edges(S, os);
      
      // pour visualiser les facettes eventuellement candidates...
      //       write_to_file_iv_border_facets(S, os);

      // pour afficher les points non selectionnes, ~bruit???
      //      write_to_file_iv_remaining_points(S, os);
    }

  os << "}" << std::endl;  

  std::cout << "-- Inventor result written." << std::endl;
}


template <class Surface>
void
write_boundaries(std::ostream& os, const Surface& S)
{  

  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Boundary_iterator Boundary_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Vertex Vertex;
  typedef typename Surface::Cell_handle Cell_handle;


  CGAL::Random random;
  for(Boundary_iterator it = S.boundaries_begin();
      it != S.boundaries_end();
      ++it) {
    double blue = random.get_double(0,1);
    os << 
      "Shape {\n"
      "appearance Appearance {\n"
      "material Material { emissiveColor 1 0 " << blue << "}}\n"
      "geometry\n"
      "IndexedLineSet {\n"
      "coord Coordinate {\n"
      "point [ " << std::endl;
    unsigned int count = 0;
    Vertex_handle first = *it;
    do {
      os << (*it)->point() << std::endl;
      ++it;
      count++;
    } while(*it != first);
    os << "]\n"
      "}\n"
      "coordIndex [\n";
    
    for(unsigned int i = 0; i < count; i++){
      os << i << ", ";
    }
    os << "0, -1\n";
    os << "]\n" 
      "}#IndexedLineSet\n"
      "}# Shape\n"; 
  }
}




template <class Surface>
void
write_to_file_vrml2(char* foutput, const Surface& S,
				  const bool& boundary, double red, double green, double blue, bool no_header)
{

  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();

  typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
 
  typedef CGAL::Triangulation_data_structure_2<CGAL::Tvb_3_2<Kernel> > TDS;

  TDS tds;

  TDS::Vertex_handle inf = AFSR::orient(tds, S);

  char foutput_tmp[100];
  std::strcpy(foutput_tmp, foutput);

  strcat(foutput_tmp, ".wrl");
  std::ofstream os(foutput_tmp, std::ios::out);

  if(os.fail())
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> file for output : " << foutput_tmp << std::endl;

  os.clear();

  CGAL::set_ascii_mode(os);
  
  if(! no_header){
    // Header.
    os << 
      "#VRML V2.0 utf8\n"
      "Background {skyColor .1 .5 .5}\n"
      "Group {\n"
      "children [\n" << std::endl;
  };

  afsr_vrml_output(tds, os, red, green, blue, inf, true);  

  if (boundary){
    write_boundaries(os, S);
  }

  typename Surface::Outlier_iterator pit = S.outliers_begin();

  if(pit != S.outliers_end()) {
    os << "Shape {\n"
      "geometry PointSet {\n"
      "coord Coordinate { point [\n";

    for(; pit != S.outliers_end(); pit++){
      os << pit->x() << " " << pit->y() << " " << pit->z() << ",\n";
    } 
    os << "] } }\n"
      "appearance Appearance {\n"
      "  material Material {\n"
      "    emissiveColor 1 0.1  0\n"
      "  }\n"
      "}\n"
      "} # Shape\n";
  }

  if(! no_header){
    os << "] # children\n"
      "} # Group\n";
  }
  std::cout << "-- wrl result written." << std::endl;
}



template <class Surface>
void
write_to_file_stl(char* foutput, const Surface& S)
{

  typedef typename Surface::Triangulation_3 Triangulation_3;
  typedef typename Surface::Finite_vertices_iterator Finite_vertices_iterator;
  typedef typename Surface::Finite_facets_iterator Finite_facets_iterator;
  typedef typename Surface::Vertex_handle Vertex_handle;
  typedef typename Surface::Cell_handle Cell_handle;
  Triangulation_3& T = S.triangulation();


  char foutput_tmp[100];
  std::strcpy(foutput_tmp, foutput);

  strcat(foutput_tmp, ".stl");
  std::ofstream os(foutput_tmp, std::ios::out);

  if(os.fail())
    std::cerr << "+++unable to open file for output" << std::endl;
  else
    std::cout << ">> file for output : " << foutput_tmp << std::endl;

  os.clear();

  CGAL::set_ascii_mode(os);
  
  // Header.
  os << "solid" << std::endl;
 
  for(Finite_facets_iterator f_it = T.finite_facets_begin(); 
      f_it != T.finite_facets_end(); 
      f_it++)
    {
      Cell_handle n, c = (*f_it).first;
      int ni, ci = (*f_it).second;

      bool selected = false;
      if(c->is_selected_facet(ci)){
	selected = true;
      } else {
	n = c->neighbor(ci);
	ni = n->index(c);
	if(n->is_selected_facet(ni)) {
	  selected = true;
	  c = n;
	  ci = ni;
	}
      }
      
      if(selected){
	int i1, i2 ,i3;

	i1 = (ci+1) & 3;
	i2 = (ci+2) & 3;
	i3 = (ci+3) & 3;

        Point p = c->vertex(i1)->point();
        Point q = c->vertex(i2)->point();
        Point r = c->vertex(i3)->point();
        // compute normal
        Vector n = CGAL::cross_product( q-p, r-p);
        Vector norm = n / sqrt( n * n);
	os << "outer loop" << std::endl;
	os << "facet normal " << norm << std::endl;
	os << "vertex " << p << std::endl;
	os << "vertex " << q << std::endl;
	os << "vertex " << r << std::endl;
      os << "endloop\nendfacet" << std::endl;
      }
      
    }

  os << "endsolid" << std::endl;

  std::cout << "-- stl result written." << std::endl;
}


//---------------------------------------------------------------------
template <class Surface>
void
write_to_file(char* foutput, const Surface& S,
			     const bool& boundary, const int& out_format,
			      double red, double green, double blue, bool no_header)
{
  switch(out_format)
    {
    case -2:
      // no output file...
      return;
    case -1:      
      write_to_file_iv(foutput, S, boundary);
      write_to_file_vrml2(foutput, S, boundary, red, green, blue, no_header);
      write_to_file_gv(foutput, S);
      write_to_file_medit(foutput, S);
      //write_to_file_ply(foutput, S);
      return;
    case 0:
      write_to_file_vrml2(foutput, S, boundary, red, green, blue, no_header);
      return;
    case 1:
      write_to_file_gv(foutput, S);
      return;
    case 2:
      write_to_file_medit(foutput, S);
      return;
    case 3:
      write_to_file_ply(foutput, S);
      return;
    case 4:
      write_to_file_iv(foutput, S, boundary);
      return;
    case 5:
      write_to_file_stl(foutput, S);
      return;
    }
}

} // namespace CGAL


#endif // CGAL_IO_ADVANCING_FRONT_SURFACE_RECONSTRUCTION_H
