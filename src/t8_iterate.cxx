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
#include <t8.h>
#include <sc_refcount.h>
#include <sc_statistics.h>
#include <t8_refcount.h>
#include <t8_forest.h>
#include <t8_iterate.h>
#include <t8_eclass.h>
#include <t8_element.h>
#include <t8_vec.h>
#include <t8_cmesh.h>
#include <t8_element_cxx.hxx>
#include <t8_forest/t8_forest_iterate.h>
#include <t8_forest/t8_forest_types.h>
#include <t8_forest/t8_forest_ghost.h>

#include <t8_default/t8_dtri.h>


/** Iterates over one face between two elements.
   * If both elements are leafs it calls a callback with these elements as input.
   * If the elements aren't neighbors it will calls the callback though.
   *
   * \param [in] forest The forest where the face and elements is in.
   * \param [in] ltreeid1 The treeID of the first element.
   * \param [in] ltreeid2 The treeID of the second element.
   * \param [in] element1 The first element.
   * \param [in] element2 The second element.
   * \param [in] face1 The faceID of the first element.
   * \param [in] face2 The faceID of the second element.
   * \param [in] leaf_elements1 An Array of all local leafs that are in the first element.
   *			 If element1 is leaf, leaf_elements1 contents only the element1.
   * \param [in] leaf_elements2 An Array of all local leafs that are in the second element.
   *			 If element2 is leaf, leaf_elements2 contents only the element2.
   * \param [in] user_data Just passed through to the callback.
   * \param [in] callback Is the Funktion that will be called between the elements and their faces. 
   */


void
t8_forest_iterate_over_one_face (t8_forest_t forest, t8_gloidx_t ltreeid1, t8_gloidx_t ltreeid2, t8_eclass_t eclass1, t8_eclass_t eclass2,
                         const t8_element_t * element1, const t8_element_t * element2, int face1, int face2,
                         t8_element_array_t * leaf_elements1, t8_element_array_t * leaf_elements2,
                         void *user_data,
                         t8_forest_iterate_over_all_face_fn callback)
{
  t8_eclass_scheme_c *ts1, *ts2;
  t8_element_t       **face_children1, **face_children2;
  int                 child_face1, child_face2, num_face_children, num_face_children1, num_face_children2, iface;
  int                *child_indices1, *child_indices2;
  size_t             *split_offsets1, *split_offsets2, indexa1, indexb1, indexa2, indexb2, elem_count1, elem_count2;
  t8_element_array_t  face_child_leafs1, face_child_leafs2;
  int				  proof;

			 
	/* Check wheter Element is Rand and set elem_count to 0 - else get length of leaf_elements*/

  if (element1 == NULL || leaf_elements1 == NULL) {
	  elem_count1 = 0;}
  else {
	  elem_count1 = t8_element_array_get_count (leaf_elements1);
  }
  
  if (element2 == NULL || leaf_elements1 == NULL) {
	  elem_count2 = 0;}
  else {
	  elem_count2 = t8_element_array_get_count (leaf_elements2);
  }
  
 /* get scheme of both Elements */
  ts1 = t8_forest_get_eclass_scheme (forest, eclass1);
  ts2 = t8_forest_get_eclass_scheme (forest, eclass2);
  
  /* Check whether Elements are leafs or rand */

  if (elem_count1 <= 1 && elem_count2 <= 1) {
    /* There is only one leaf left, we check whether it is the same as element
     * and if so call the callback function */
    proof = 1;
	if (element1 == NULL) {
		proof = 0;
	}
	else { 
		proof = ts1->t8_element_compare (element1, t8_element_array_index_locidx (leaf_elements1, 0));
	}
	if (1-proof == 1){
        proof = 1;
		if (element2 == NULL) {
			proof = 0;
		}
		else { 
			proof = ts2->t8_element_compare (element2, t8_element_array_index_locidx (leaf_elements2, 0));
		}
		if (1-proof == 1){ 
          /* The element is the leaf, we are at the last stage of the recursion
          * and can call the callback. */
          (void) callback (forest, ltreeid1, ltreeid2, element1, element2, face1, face2, user_data);
        }
    }
	return;
  }


 /* Check wether Element 1 is leaf or rand */
  if (elem_count1 <= 1) {
    /* There is only one leaf left, we check whether it is the same as element */
	proof = 1;
	if (element1 == NULL) {
		proof = 0;
	}
	else { 
		proof = ts1->t8_element_compare (element1, t8_element_array_index_locidx (leaf_elements1, 0));
	}
	if (1-proof == 1){
          /* Enter the recursion */
          /* We compute all face children of E, compute their leaf arrays and
           * call iterate_over_one_face */
          /* allocate the memory to store the face children */
          num_face_children2 = ts2->t8_element_num_face_children (element2, face2);
          face_children2 = T8_ALLOC (t8_element_t *, num_face_children2);
          ts2->t8_element_new (num_face_children2, face_children2);
          /* Memory for the child indices of the face children */
          child_indices2 = T8_ALLOC (int, num_face_children2);
          /* Memory for the indices that split the leaf_elements array */
          split_offsets2 = T8_ALLOC (size_t, ts2->t8_element_num_children (element2) + 1);
          /* Compute the face children */
          ts2->t8_element_children_at_face (element2, face2, face_children2,
                                           num_face_children2, child_indices2);
          /* Split the leafs array in portions belonging to the children of element */
          t8_forest_split_array (element2, leaf_elements2, split_offsets2);
          for (iface = 0; iface < num_face_children2; iface++) {
            /* Check if there are any leaf elements for this face child */
            indexa2 = split_offsets2[child_indices2[iface]];     /* first leaf of this face child */
            indexb2 = split_offsets2[child_indices2[iface] + 1]; /* first leaf of next child */
            if (indexa2 < indexb2) {
              /* There exist leafs of this face child in leaf_elements,
               * we construct an array of these leafs */
              t8_element_array_init_view (&face_child_leafs2, leaf_elements2, indexa2,
                                          indexb2 - indexa2);
              /* Compute the corresponding face number of this face child */
              child_face2 = ts2->t8_element_face_child_face (element2, face2, iface);
              /* Enter the recursion */
              t8_forest_iterate_over_one_face (forest, ltreeid1, ltreeid2, eclass1, eclass2, element1, face_children2[iface],
                                       face1, child_face2, leaf_elements1, &face_child_leafs2, user_data,
                                       callback);

            }
          }
          /* clean-up */
          ts2->t8_element_destroy (num_face_children2, face_children2);
          T8_FREE (face_children2);
          T8_FREE (child_indices2);
          T8_FREE (split_offsets2);
          return;
    }
  }
  
	/* Check wether Element 2 is leaf or rand */
  if (elem_count2 <= 1) {
    /* There is only one leaf left, we check whether it is the same as element */
	proof = 1;
	if (element2 == NULL) {
		proof = 0;
	}
	else { 
		proof = ts2->t8_element_compare (element2, t8_element_array_index_locidx (leaf_elements2, 0));
	}
	if (1-proof == 1){
          /* Enter the recursion */
          /* We compute all face children of E, compute their leaf arrays and
           * call iterate_over_one_face */
          /* allocate the memory to store the face children */
          num_face_children1 = ts1->t8_element_num_face_children (element1, face1);
          face_children1 = T8_ALLOC (t8_element_t *, num_face_children1);
          ts1->t8_element_new (num_face_children1, face_children1);
          /* Memory for the child indices of the face children */
          child_indices1 = T8_ALLOC (int, num_face_children1);
          /* Memory for the indices that split the leaf_elements array */
          split_offsets1 = T8_ALLOC (size_t, ts1->t8_element_num_children (element1) + 1);
          /* Compute the face children */
          ts1->t8_element_children_at_face (element1, face1, face_children1,
                                           num_face_children1, child_indices1);
          /* Split the leafs array in portions belonging to the children of element */
          t8_forest_split_array (element1, leaf_elements1, split_offsets1);
          for (iface = 0; iface < num_face_children1; iface++) {
            /* Check if there are any leaf elements for this face child */
            indexa1 = split_offsets1[child_indices1[iface]];     /* first leaf of this face child */
            indexb1 = split_offsets1[child_indices1[iface] + 1]; /* first leaf of next child */
            if (indexa1 < indexb1) {
              /* There exist leafs of this face child in leaf_elements,
               * we construct an array of these leafs */
              t8_element_array_init_view (&face_child_leafs1, leaf_elements1, indexa1,
                                          indexb1 - indexa1);
              /* Compute the corresponding face number of this face child */
              child_face1 = ts1->t8_element_face_child_face (element1, face1, iface);
              /* Enter the recursion */
              t8_forest_iterate_over_one_face (forest, ltreeid1, ltreeid2, eclass1, eclass2, face_children1[iface], element2,
                                       child_face1, face2, &face_child_leafs1, leaf_elements2, user_data,
                                       callback);

            }
          }
          /* clean-up */
          ts1->t8_element_destroy (num_face_children1, face_children1);
          T8_FREE (face_children1);
          T8_FREE (child_indices1);
          T8_FREE (split_offsets1);
          return;
    }
  }
  
  /* Element 1 and Element 2 aren't rand or leafs - so both must be devided into childelments. */

    /* Enter the recursion */
    /* We compute all face children of E, compute their leaf arrays and
     * call iterate_over_one_face */
    /* allocate the memory to store the face children */
    num_face_children1 = ts1->t8_element_num_face_children (element1, face1);
    face_children1 = T8_ALLOC (t8_element_t *, num_face_children1);
    ts1->t8_element_new (num_face_children1, face_children1);
    /* Memory for the child indices of the face children */
    child_indices1 = T8_ALLOC (int, num_face_children1);
    /* Memory for the indices that split the leaf_elements array */
    split_offsets1 = T8_ALLOC (size_t, ts1->t8_element_num_children (element1) + 1);
    /* Compute the face children */
    ts1->t8_element_children_at_face (element1, face1, face_children1,
                                     num_face_children1, child_indices1);
    /* Split the leafs array in portions belonging to the children of element */
    t8_forest_split_array (element1, leaf_elements1, split_offsets1);

    num_face_children2 = ts2->t8_element_num_face_children (element2, face2);
    face_children2 = T8_ALLOC (t8_element_t *, num_face_children2);
    ts2->t8_element_new (num_face_children2, face_children2);
    /* Memory for the child indices of the face children */
    child_indices2 = T8_ALLOC (int, num_face_children2);
    /* Memory for the indices that split the leaf_elements array */
    split_offsets2 = T8_ALLOC (size_t, ts2->t8_element_num_children (element2) + 1);
    /* Compute the face children */
    ts2->t8_element_children_at_face (element2, face2, face_children2,
                                     num_face_children2, child_indices2);
    /* Split the leafs array in portions belonging to the children of element */
    t8_forest_split_array (element2, leaf_elements2, split_offsets2);

    if(num_face_children1 == num_face_children2){
        num_face_children = num_face_children1;
    }
    else{
        num_face_children = 0;
        printf("Fehler verschieden viele Kinder!\n");
    }

    for (iface = 0; iface < num_face_children; iface++) {
      /* Check if there are any leaf elements for this face child */
      indexa1 = split_offsets1[child_indices1[iface]];     /* first leaf of this face child */
      indexb1 = split_offsets1[child_indices1[iface] + 1]; /* first leaf of next child */
      indexa2 = split_offsets2[child_indices2[iface]];     /* first leaf of this face child */
      indexb2 = split_offsets2[child_indices2[iface] + 1]; /* first leaf of next child */
      if (indexa1 < indexb1 && indexa2 < indexb2) {
        /* There exist leafs of this face child in leaf_elements,
         * we construct an array of these leafs */
        t8_element_array_init_view (&face_child_leafs1, leaf_elements1, indexa1,
                                    indexb1 - indexa1);
        /* Compute the corresponding face number of this face child */
        child_face1 = ts1->t8_element_face_child_face (element1, face1, iface);
        t8_element_array_init_view (&face_child_leafs2, leaf_elements2, indexa2,
                                    indexb2 - indexa2);
        /* Compute the corresponding face number of this face child */
        child_face2 = ts2->t8_element_face_child_face (element2, face2, iface);
        /* Enter the recursion */
        t8_forest_iterate_over_one_face (forest, ltreeid1, ltreeid2, eclass1, eclass2, face_children1[iface],face_children2[iface],
                                 child_face1, child_face2, &face_child_leafs1, &face_child_leafs2, user_data,
                                 callback);
      }
    }
    /* clean-up */
    ts1->t8_element_destroy (num_face_children1, face_children1);
    ts2->t8_element_destroy (num_face_children2, face_children2);
    T8_FREE (face_children1);
    T8_FREE (face_children2);
    T8_FREE (child_indices1);
    T8_FREE (child_indices2);
    T8_FREE (split_offsets1);
    T8_FREE (split_offsets2);
}





/** Finds all adjacent childelements in the Element and calls "iterate_over_one_face" between each pair.
   * If the element is leaf nothing will happen.
   * TODO: Write schemefunktion that output all adjacent childpairs, for fast runtime.
   *
   * \param [in] forest The forest where the element is in.
   * \param [in] ltreeid The treeID of the element.
   * \param [in] element The element.
   * \param [in] ts The Scheme of the element.
   * \param [in] leaf_elements An Array of all local leafs that are in the element.
   *			 Could be empty.
   * \param [in] ghost_leaf_elements An Array of all ghost leafs that are in the element.
   *			 Could be emtpy.
   * \param [in] user_data Just passed through to the the funktion "iterate_over_one_face".
   * \param [in] callback Just passed through to the the funktion "iterate_over_one_face".
   */

static void
t8_forest_iterate_over_all_inner_faces_of_one_element (t8_forest_t forest, t8_locidx_t ltreeid,
                            t8_eclass_t eclass, t8_element_t * element,
                            t8_eclass_scheme_c *ts,
                            t8_element_array_t *leaf_elements,
							t8_element_array_t *ghost_leaf_elements,
                            void *user_data,
							t8_forest_iterate_over_all_face_fn callback)
{
	
  t8_element_t       **children;
  int                 num_children, ichild, jchild;
  size_t             *split_offsets, *ghost_split_offsets, indexa1, indexb1, indexa2, indexb2;
  t8_element_array_t  child_leafs1, child_leafs2;
  size_t              elem_count;
  int                 iface, face_count;
  int				  ghost_ex;
  int				  ancestor_id, level;
  #if 0
	int 			  inner_faces, num_inner_faces;
  #endif
  #if 1
	t8_element_t		 *neigh;
	int 		          neigh_face[1];
  #endif
	/* Check for right input */
	
  T8_ASSERT (t8_forest_is_committed (forest));
  T8_ASSERT (0 <= ltreeid
             && ltreeid < t8_forest_get_num_local_trees (forest));
			 
	/* get the level of element*/
	level = ts->t8_element_level (element); 
	
	
	/* check whether there are ghost elements in Element */
	ghost_ex = 1;
	if(ghost_leaf_elements == NULL){
		ghost_ex = 0;
	}

	/* check whether element is leaf */

  elem_count = t8_element_array_get_count (leaf_elements);
  if (elem_count <= 1) {
    /* There are no children inside, so we have nothing to do */
    return;
  }
  
    /* We compute all children of Element, compute their leaf arrays and
     * call "iterate_over_one_face" between two of them if their adjacent and call "iterate_over_all_inner_faces_of_one_element" on each child. */
    /* allocate the memory to store the children */
    num_children = ts->t8_element_num_children (element);
    children = T8_ALLOC (t8_element_t *, num_children);
    ts->t8_element_new (num_children, children);
    /* Memory for the indices that split the leaf_elements array */
    split_offsets = T8_ALLOC (size_t, num_children + 1);
    if(ghost_ex){ghost_split_offsets = T8_ALLOC (size_t, num_children + 1);}
    /* Compute the children */
    ts->t8_element_children (element, num_children, children);
    /* Split the leafs array in portions belonging to the children of element */
    t8_forest_split_array (element, leaf_elements, split_offsets);
	/* Split the leafs array of ghost elements in portions belonging to the children of element only if ghost exists. */
    if(ghost_ex){t8_forest_split_array (element, ghost_leaf_elements, ghost_split_offsets);}
	
	#if 0
	num_inner_faces = ts->t8_get_num_inner_faces(element);
	for (inner_faces = 0; inner_faces < num_inner_faces; inner_faces++){
		face_pair = ts->t8_get_face_pair(element, inner_faces);
		ichild = face_pair[0];
		jchild = face_pair[1]
		indexa1 = split_offsets[ichild];   /* first leaf of this child */
		indexb1 = split_offsets[ichild + 1];       /* first leaf of next child */
		indexa2 = split_offsets[jchild];   /* first leaf of this child */
		indexb2 = split_offsets[jchild + 1];       /* first leaf of next child */
		if (indexa1 < indexb1) {
			if (indexb1 < indexb2) {
				/* There exist leafs of this child in leaf_elements,
				 * we construct an array of these leafs */
				t8_element_array_init_view (&child_leafs1, leaf_elements, indexa1, indexb1 - indexa1);
				t8_element_array_init_view (&child_leafs2, leaf_elements, indexa2, indexb2 - indexa2);
				/* call iterate_over_one_face between two adjacent children in element */
				t8_forest_iterate_over_one_face (forest, t8_forest_global_tree_id(forest, ltreeid), t8_forest_global_tree_id(forest, ltreeid), eclass, eclass, children[ichild], neigh, iface, *neigh_face,
																&child_leafs1, &child_leafs2, user_data,
																callback);
			}
			else if(ghost_ex){
				/* Check if there are any leaf elements in ghost for this child */
				indexa2 = ghost_split_offsets[jchild];   /* first leaf of this child */
				indexb2 = ghost_split_offsets[jchild + 1];       /* first leaf of next child */
				if (indexa2 < indexb2) {
					/* There exist leafs of this child in leaf_elements,
					* we construct an array of these leafs */
					t8_element_array_init_view (&child_leafs2, ghost_leaf_elements, indexa2, indexb2 - indexa2);
					/* call iterate_over_one_face between two adjacent children in element (second is in ghost)*/
					t8_forest_iterate_over_one_face (forest, t8_forest_global_tree_id(forest, ltreeid), t8_forest_global_tree_id(forest, ltreeid), eclass, eclass, children[ichild], neigh, iface, *neigh_face,
													&child_leafs1, &child_leafs2, user_data,
													callback);
				}
			}
		}
	}
	
	#endif
	
	
	
	#if 1
	/* allocate memory for neighbor */
    ts->t8_element_new (1, &neigh);
    for (ichild = 0; ichild < num_children; ichild++) {
		ancestor_id = ts->t8_element_ancestor_id (children[ichild], level);
		/* Check if there are any leaf elements for this child */
		indexa1 = split_offsets[ichild];   /* first leaf of this child */
		indexb1 = split_offsets[ichild + 1];       /* first leaf of next child */
		if (indexa1 < indexb1) {
			/* There exist leafs of this child in leaf_elements,
			 * we construct an array of these leafs */
			t8_element_array_init_view (&child_leafs1, leaf_elements, indexa1,
									   indexb1 - indexa1);
			face_count = ts->t8_element_num_faces(children[ichild]);
			/* Iterate over all faces and construct neighbor */
			for (iface = 0; iface < face_count; iface++) {
				if (ts->t8_element_face_neighbor_inside (children[ichild],
												 neigh,
												 iface,
												 neigh_face)){
					jchild = ts->t8_element_child_id (neigh);
					/* check wether neighbor is inside element */
					if (ichild < jchild && (ancestor_id == ts->t8_element_ancestor_id (neigh, level))){
						/* Check if there are any leaf elements for this child */
						indexa2 = split_offsets[jchild];   /* first leaf of this child */
						indexb2 = split_offsets[jchild + 1];       /* first leaf of next child */
						if (indexa2 < indexb2) {
						/* There exist leafs of this child in leaf_elements,
						* we construct an array of these leafs */
						t8_element_array_init_view (&child_leafs2, leaf_elements, indexa2, indexb2 - indexa2);
							/* call iterate_over_one_face between two adjacent children in element */
							t8_forest_iterate_over_one_face (forest, t8_forest_global_tree_id(forest, ltreeid), t8_forest_global_tree_id(forest, ltreeid), eclass, eclass, children[ichild], neigh, iface, *neigh_face,
															&child_leafs1, &child_leafs2, user_data,
															callback);
						}
						/* same thing if ghost exist. confirm whether neigh child elemet is in ghost.*/
						if(ghost_ex){
							/* Check if there are any leaf elements in ghost for this child */
							indexa2 = ghost_split_offsets[jchild];   /* first leaf of this child */
							indexb2 = ghost_split_offsets[jchild + 1];       /* first leaf of next child */
							if (indexa2 < indexb2) {
							/* There exist leafs of this child in leaf_elements,
							* we construct an array of these leafs */
							t8_element_array_init_view (&child_leafs2, ghost_leaf_elements, indexa2, indexb2 - indexa2);
								/* call iterate_over_one_face between two adjacent children in element (second is in ghost)*/
								t8_forest_iterate_over_one_face (forest, t8_forest_global_tree_id(forest, ltreeid), t8_forest_global_tree_id(forest, ltreeid), eclass, eclass, children[ichild], neigh, iface, *neigh_face,
																&child_leafs1, &child_leafs2, user_data,
																callback);
							}
						}
					}
				}
			}
        }
	}
	ts->t8_element_destroy (1, &neigh);
	#endif
	
	/* Iterate over all children to enter the recursion */
	for (ichild = 0; ichild < num_children; ichild++) {
		/* Check if there are any leaf elements for this child */
		indexa1 = split_offsets[ichild];   /* first leaf of this child */
		indexb1 = split_offsets[ichild + 1];       /* first leaf of next child */
		if (indexa1 < indexb1) {
			/* There exist leafs of this child in leaf_elements,
			 * we construct an array of these leafs */
			t8_element_array_init_view (&child_leafs1, leaf_elements, indexa1,
									   indexb1 - indexa1);
			/* There exist leafs of this child in leaf_elements in ghost,
			 * we construct an array of these leafs */
			if(ghost_ex){
				indexa2 = ghost_split_offsets[ichild];   /* first leaf of this child */
				indexb2 = ghost_split_offsets[ichild + 1];       /* first leaf of next child */
				if (indexa2 < indexb2) {
					/* There exist leafs in ghost of this child in ghost_leaf_elements,
					 * we construct an array of these leafs */
					t8_element_array_init_view (&child_leafs2, ghost_leaf_elements, indexa2,
												indexb2 - indexa2);
					/* Enter the recursion with Ghostelements */
					t8_forest_iterate_over_all_inner_faces_of_one_element (forest, ltreeid, eclass, children[ichild],
												ts, &child_leafs1, &child_leafs2,
												user_data, callback);
				}
				else{
					/* Enter the recursion without Ghostelements*/
					t8_forest_iterate_over_all_inner_faces_of_one_element (forest, ltreeid, eclass, children[ichild],
												ts, &child_leafs1, NULL,
												user_data, callback);
				}
			}
			else{
				/* Enter the recursion without Ghostelements */
				t8_forest_iterate_over_all_inner_faces_of_one_element (forest, ltreeid, eclass, children[ichild],
												ts, &child_leafs1, NULL,
												user_data, callback);
			}						   				
		}
	}
    /* clean-up */
    ts->t8_element_destroy (num_children, children);
    T8_FREE (split_offsets);
    if(ghost_ex){T8_FREE (ghost_split_offsets);}
	T8_FREE (children);
	return;
}

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
t8_forest_iterate_over_all_faces(const t8_forest_t forest,
                         void *user_data,
                         const t8_forest_iterate_over_all_face_fn callback){

	t8_forest_t			forest_with_ghost;
	t8_locidx_t         num_local_trees, itree, neigh_treeid, neigh_local_treeid;
	t8_eclass_t         eclass, neigh_eclass;
	t8_eclass_scheme_c *ts, *neigh_scheme;
	t8_element_t       *nca, *first_el, *last_el, *big_nca;
	t8_element_t       *nca_neigh;
	t8_element_array_t *leaf_elements, *neigh_leaf_elements, *neigh_elements_ghost;
	t8_locidx_t			iface, face_count;
	t8_locidx_t			neigh_face[1];
	t8_element_t       *neigh;
	int					ex_big_nca;
	
	
	/* Initialisate forest_with_ghost, partition it and set ghost for multiprocessing. */

	t8_forest_init (&forest_with_ghost);
	t8_forest_ref (forest);
	t8_forest_set_partition (forest_with_ghost, forest, 0);
	t8_forest_set_ghost (forest_with_ghost, 1, T8_GHOST_FACES);
	t8_forest_commit (forest_with_ghost);
	/* Iterate over all local trees */
	num_local_trees = t8_forest_get_num_local_trees (forest_with_ghost);
	for (itree = 0; itree < num_local_trees; itree++) {		
		ex_big_nca = 0;
		
		/* Get the element class, scheme and leaf elements of this tree */
		eclass = t8_forest_get_eclass (forest_with_ghost, itree);
		ts = t8_forest_get_eclass_scheme (forest_with_ghost, eclass);
		leaf_elements = t8_forest_tree_get_leafs (forest_with_ghost, itree);

		/* assert for empty tree */
		T8_ASSERT (t8_element_array_get_count (leaf_elements) >= 0);

		/* Get the first and last leaf of this tree */
		first_el = t8_element_array_index_locidx (leaf_elements, 0);
		last_el = t8_element_array_index_locidx (leaf_elements, t8_element_array_get_count (leaf_elements) - 1);
	
		/* Compute their nearest common ancestor, this is the local rootelement */
		ts->t8_element_new (1, &nca);
		ts->t8_element_nca (first_el, last_el, nca);
		
		/* Iterate over all faces of the local rootelement */
		face_count = ts->t8_element_num_faces(nca);
		for (iface = 0; iface < face_count; iface++){
			/* Get the eclass of neigh tree and neigh_scheme */
			neigh_eclass = t8_forest_element_neighbor_eclass (forest_with_ghost, itree, nca, iface);
			neigh_scheme = t8_forest_get_eclass_scheme (forest_with_ghost, neigh_eclass);
			neigh_scheme->t8_element_new (1, &neigh);
			neigh_treeid = t8_forest_element_face_neighbor (forest_with_ghost, itree, nca, neigh, neigh_scheme, iface, neigh_face);
			/* In case element is with iface on Boundary call "iterate_over_one_face" on element and Boundary. */
			if(neigh_treeid == -1){
				t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), t8_forest_global_tree_id(forest_with_ghost, itree), eclass, eclass, nca, NULL, iface, 0, leaf_elements, leaf_elements, user_data, callback);
			}
			/* In case the rootelement are in the same tree (only possible if MPI is used) */
			else if(neigh_treeid == t8_forest_global_tree_id(forest_with_ghost, itree)){
				if(!ex_big_nca) {
					/* get rootelement of nca and neigh */
					ts->t8_element_new (1, &big_nca);
					ts->t8_element_nca (nca, neigh, big_nca);
					ex_big_nca = 1;			
					/* NCA is the same as the common ancestor of nca and neigh, so do not iterate in big_nca but in NCA */
					if (!ts->t8_element_compare(big_nca, nca)){
						ex_big_nca = 0;
					}
					if(!ex_big_nca) {
						ts->t8_element_destroy (1, &big_nca);
					}
				}
			}
			else {
				/* neigh and NCA are in different trees */
				/* check wheter neigh is in local tree */
				neigh_local_treeid = t8_forest_get_local_id (forest_with_ghost, neigh_treeid);
				if (neigh_local_treeid != -1){
					/* Get the leaf elements of neigh element */
					neigh_leaf_elements = t8_forest_tree_get_leafs (forest_with_ghost, neigh_local_treeid);
					/* Get the first and last leaf of this tree */
					first_el = t8_element_array_index_locidx (neigh_leaf_elements, 0);
					last_el = t8_element_array_index_locidx (neigh_leaf_elements, t8_element_array_get_count (neigh_leaf_elements) - 1);
				
					/* Compute their nearest common ancestor */
					neigh_scheme->t8_element_new (1, &nca_neigh);
					neigh_scheme->t8_element_nca (first_el, last_el, nca_neigh);
					
					if(ts->t8_element_level (nca) == neigh_scheme->t8_element_level (nca_neigh)){
						/* it says that nca_neigh = neigh */
						if(t8_forest_global_tree_id(forest_with_ghost,itree) < neigh_treeid) {
							/* Iterate over two Elements in different trees same process */
							t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), neigh_treeid, eclass, neigh_eclass, nca, nca_neigh, iface, neigh_face[0], 
															leaf_elements, neigh_leaf_elements, user_data, callback);
							
							/* Iterate over two Elements in different trees in ghost - use same order */
							if(t8_forest_ghost_num_trees (forest_with_ghost) != 0){
								/* Get the leaf elements of neigh element in ghost*/
								neigh_elements_ghost = t8_forest_ghost_get_tree_elements (forest_with_ghost, neigh_treeid);
						
								/* Iterate over two Elements in different trees in ghost */
								t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), neigh_treeid, eclass, neigh_eclass, nca, nca_neigh, iface, neigh_face[0], 
																	leaf_elements, neigh_elements_ghost, user_data, callback);
							}
						}
					}
					else if(ts->t8_element_level (nca) < neigh_scheme->t8_element_level (nca_neigh)){
						/* nca and neigh must be bigger than nca_neigh. */
						/* Iterate over two Elements in different trees same process */
						t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), neigh_treeid, eclass, neigh_eclass, nca, neigh, iface, neigh_face[0], 
															leaf_elements, neigh_leaf_elements, user_data, callback);
						
						/* Iterate over two Elements in different trees in ghost - use same order */
						if(t8_forest_ghost_num_trees (forest_with_ghost) > 0){
							/* Get the leaf elements of neigh element in ghost*/
							neigh_elements_ghost = t8_forest_ghost_get_tree_elements (forest_with_ghost, t8_forest_ghost_get_ghost_treeid (forest_with_ghost, neigh_treeid));
						
							/* Iterate over two Elements in different trees in ghost */
							t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), neigh_treeid, eclass, neigh_eclass, nca, neigh, iface, neigh_face[0], 
																leaf_elements, neigh_elements_ghost, user_data, callback);
						}
					}
					neigh_scheme->t8_element_destroy (1, &nca_neigh);
				}
				/* Neigh must be full in ghost */
				else {
					if(t8_forest_global_tree_id(forest_with_ghost, itree) < neigh_treeid) {
						/* Get the leaf elements of neigh element in ghost*/
						neigh_elements_ghost = t8_forest_ghost_get_tree_elements (forest_with_ghost, itree);
							
						/* Iterate over two Elements in different trees in ghost */
						t8_forest_iterate_over_one_face (forest_with_ghost, t8_forest_global_tree_id(forest_with_ghost, itree), neigh_treeid, eclass, neigh_eclass, nca, neigh, iface, neigh_face[0], 
															leaf_elements, neigh_elements_ghost, user_data, callback);
					}
				}
			}
		neigh_scheme->t8_element_destroy (1, &neigh);
		}
		
		/* get ghostelements only if there are ghosts */
		if(t8_forest_ghost_num_trees (forest_with_ghost) > 0){
			neigh_elements_ghost = t8_forest_ghost_get_tree_elements (forest_with_ghost, itree);
		}
		else{
			neigh_elements_ghost = NULL;
		}
		
		/* Search inner faces big_nca if nessesary */
		if (ex_big_nca){
			t8_forest_iterate_over_all_inner_faces_of_one_element (forest_with_ghost, itree, eclass, big_nca, ts, 
																	leaf_elements, neigh_elements_ghost, user_data, callback);
			ts->t8_element_destroy (1, &big_nca);
		}
		else{
			/* Serach inner faces of nca. If a big_nca exist. NCA is searched through with it. */
			t8_forest_iterate_over_all_inner_faces_of_one_element (forest_with_ghost, itree, eclass, nca, ts, 
																	leaf_elements, neigh_elements_ghost, user_data, callback);
		}
		
		ts->t8_element_destroy (1, &nca);
	}
	t8_forest_unref (&forest_with_ghost);
}