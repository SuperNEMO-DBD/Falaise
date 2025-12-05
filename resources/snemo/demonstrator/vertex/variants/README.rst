==============================================================================
Generation of the current list of vertex generators for the variant system
==============================================================================

:author: F.Mauger
:date: 2025-12-05
	 
.. code:: shell
	  
   $ cd resources/snemo/demonstrator/vertex/variants
   $ bash prepare_csv.sh
   $ less _test_vertexes_generators.csv
..

Then     compare     the     list    in     the     generated     file
``_test_vertexes_generators.csv``  with  the   list  in  the  official
``./variants/X.Y/vertexes_generators.csv``   (where  ``X.Y``   is  the
current version  of the  vertex generator manager  component).  Update
the  official  list in  ``./variants/X.Y/vertexes_generators.csv``  to
take  into account  new vertex  generators detected  from the  list of
vertex generators configuration files.

The ``./variants/X.Y/vertexes_generators.csv`` uses the following format:

 * Blank lines and lines starting with '#' are ignored.
 * CSV layout with separator character ':' (colon punctuation character)

   - Column 1 : name of the vertex generator
   - Column 2 : single line description of the vertex generator
   - Column 3 : name of the *variant group* of options for setting long range dependencies in the general variant system
   - Column 4 : *local variant* triggered by the generator if selected
   - Column 5 : metadata (specification of the *display rank* for the GUI)

Some  specific  *variant groups*  ("OpticalModule",  "Calibration"...)
are  thus specified  (third  column)  in order  to  define long  range
dependencies between  variant options  in distinct  variant registries
("geometry", "vertexes") and also help the layout of the GUI.
 
The fourth  column contains the name  of the *local variant*  (if any)
which is automatically triggered by the selection of the generator.

In  some case,  a specific  rank (fifth  column) is  provided for  the
variant GUI.




.. end
   
