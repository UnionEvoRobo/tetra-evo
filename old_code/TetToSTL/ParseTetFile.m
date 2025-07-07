function [verts, tets] = ParseTetFile(infilename)

    infile = fopen(infilename,'r')
    
    tets = [];
    verts = [];
    
    while 1
       curline = fgetl(infile)
       if (~ischar(curline) || (length(curline) == 0)), break, end
       if (curline(1) == 't')
           vertsOfTet = sscanf(curline(2:length(curline)),'%d');
           tets = [tets; vertsOfTet'];
       elseif (curline(1) == 'v')
           coordsOfVert = sscanf(curline(2:length(curline)),'%f')
           verts = [verts; coordsOfVert'];
       end
    end
   
    
    fclose(infile)