#include <t8_forest.h>
#include <t8.h>
#include <t8_element_cxx.hxx>
#include <sc_refcount.h>
#include <t8_eclass.h>
#include <t8_element.h>
#include <t8_iterate.h>



/*neue datei - merge..*/

										   
												   

/*erstelle mehrere einfache (Level = 1) Forest's mit verschiedenen Elemententyp und prüfe durch um herauszufinden, wo welche Seiten liegen.*/
/*(funktioniert aber auch mit höheren Leveln, geht aber in die tiefste Ebene und prüft nicht jede Ebene durch) */
/*Ausgabe: Zu jedem Element von 0 bis elem_count die Vergleich von Seitenzahl des Elements mit dem der Ältern (bzw. falls vorhanden).*/
												   
void
t8_seiten (t8_forest_t forest){
  t8_locidx_t         num_local_trees, itree;

  /*rekusion über alle Bäume*/
  num_local_trees = t8_forest_get_num_local_trees (forest);
  for (itree = 0; itree < num_local_trees; itree++) {
    t8_seiten_tree (forest, itree);
  }
}

static void
t8_seiten_tree (t8_forest_t forest, t8_locidx_t ltreeid)
{
  t8_eclass_t         eclass;
  t8_eclass_scheme_c *ts;
  t8_element_array_t *leaf_elements;
  size_t             elem_count;
  int				 ielem, face_count, parent_face;

  /* Get the element class, scheme, leaf elements and number of leaf elements of this tree */
  eclass = t8_forest_get_eclass (forest, ltreeid);
  ts = t8_forest_get_eclass_scheme (forest, eclass);
  leaf_elements = t8_forest_tree_get_leafs (forest, ltreeid);
  /*rekusion über alle Elemente*/
  elem_count = t8_element_array_get_count (leaf_elements);
  for (ielem = 0; ielem < elem_count; ielem++){
	  /*rekusion über alle Seiten*/
	  face_count = t8_element_num_faces(leaf_elements(ielem));
	  for(iface = 0; iface < face_count; iface++){
		  /*berechne die Seite des Parentelements*/
		  parent_face = t8_element_face_parent_face (ielem, iface);
		  
		  /*Gib Ergebnis aus*/
		  printf("Für das Element %i ist die Seite %i bei dem Parentelement die Seite %i. \n", ielem, iface, parent_face);
	  }
  }  
}
