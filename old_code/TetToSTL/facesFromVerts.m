function facesOut = facesFromVerts(verts)

face1 = sort([verts(1) verts(2) verts(3)]);
face2 =  sort([verts(1) verts(2) verts(4)]);
face3 = sort([verts(1) verts(3) verts(4)]);
face4 = sort([ verts(2) verts(3) verts(4)]);
            
facesOut = [face1;face2;face3;face4];

           
           