// Copyright (c) 2005  INRIA Sophia-Antipolis (France) 
// All rights reserved.
//
// Authors : Monique Teillaud <Monique.Teillaud@sophia.inria.fr>
//           Sylvain Pion     <Sylvain.Pion@sophia.inria.fr>
// 
// Partially supported by the IST Programme of the EU as a Shared-cost
// RTD (FET Open) Project under Contract No  IST-2000-26473 
// (ECG - Effective Computational Geometry for Curves and Surfaces) 
// and a STREP (FET Open) Project under Contract No  IST-006413 
// (ACS -- Algorithms for Complex Shapes)

#ifndef CGAL_CURVED_KERNEL_FUNCTIONS_ON_LINE_2_H
#define CGAL_CURVED_KERNEL_FUNCTIONS_ON_LINE_2_H

#include <CGAL/global_functions_on_roots_and_polynomial_1_2_and_2_2.h>

namespace CGAL {
namespace LinearFunctors {

  template < class CK >
  typename CK::Polynomial_1_2
  get_equation( const typename CK::Line_2 & L )
  {
    typedef typename CK::RT RT;
 
    return typename CK::Polynomial_1_2(L.a(),L.b(),L.c());
  }
  
  template < class CK >
  typename CK::Line_2  
  construct_line_2 ( const typename CK::Polynomial_1_2 &eq )
  {
    return typename CK::Line_2(eq[2],eq[1],eq[0]); 
  }
  

  template< class CK, class OutputIterator>
    OutputIterator
    intersect_2( const typename CK::Line_2 & l,
			       const typename CK::Circle_2 & c,
			       OutputIterator res )
  {
    typedef typename CK::Polynomial_1_2 Equation_line;
    typedef typename CK::Polynomial_for_circles_2_2 Equation_circle; 
    typedef typename CK::Root_for_circles_2_2 Root_for_circles_2_2;

    Equation_line e1 = get_equation<CK>(l);
    Equation_circle e2 = CGAL::get_equation<CK>(c);
    
    typedef std::vector
      < std::pair 
      < Root_for_circles_2_2, 
      unsigned > > 
      solutions_container;
    solutions_container solutions;
    CGAL::solve<typename CK::Algebraic_kernel>
      ( e1,e2, std::back_inserter(solutions) ); // to be optimized
    
    typedef typename CK::Circular_arc_point_2 Circular_arc_point_2;

    for ( typename solutions_container::iterator it = solutions.begin(); 
	  it != solutions.end(); ++it )
      {
	*res++ = make_object
	  (std::make_pair
	   (Circular_arc_point_2(it->first), it->second ));
      }

    return res;
  }


} // namespace LinearFunctors
} // namespace CGAL
#endif // CGAL_CURVED_KERNEL_FUNCTIONS_ON_LINE_2_H
