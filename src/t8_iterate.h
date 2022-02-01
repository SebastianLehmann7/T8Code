/*
  This file is part of t8code.
  t8code is a C library to manage a collection (a forest) of multiple
  connected adaptive space-trees of general element types in parallel.

  Copyright (C) 2015 the developers

  t8code is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  t8code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with t8code; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.


*/


#ifndef T8_ITERATE_H
#define T8_ITERATE_H

#include <t8.h>
#include <t8_forest.h>
#include <t8_forest/t8_forest_iterate.h>

T8_EXTERN_C_BEGIN ();

/** typedef fuer den callback in t8_forest_iterate_over_all_faces */

typedef int         
(*t8_forest_iterate_over_all_face_fn) (t8_forest_t forest, t8_gloidx_t ltreeid1, t8_gloidx_t ltreeid2,
										const t8_element_t * element1,const t8_element_t * element2, 
										int face1, int face2, void *user_data);

														
														
/** Iterates over all faces in one forest.
   * Calls "iterate_over_one_face" between two rootelements or an element and the roots.
   *		With multiprocession the rootelements could be in the same tree.
   * Calls "iterate_over_all_inner_faces_of_one_element" on all rootelements.
   *
   * \param [in] forest The forest which should be considerd.
   * \param [in] user_data Just passed through to the the funktion "iterate_over_one_face" or "iterate_over_all_inner_faces_of one_element".
   * \param [in] callback Just passed through to the the funktion "iterate_over_one_face" or "iterate_over_all_inner_faces_of one_element".
   */

/** NOT funktional for Prisms and Pyramids, because schemefunction t8_element_nca() isn't implemented yet.
  * TODO: Implement NCA for Prims and Pyramids 
  */
   
void
t8_forest_iterate_over_all_faces(t8_forest_t forest,
                         void *user_data,
                         t8_forest_iterate_over_all_face_fn callback);



T8_EXTERN_C_END ();

#endif /* !T8_ITERATE_H */
