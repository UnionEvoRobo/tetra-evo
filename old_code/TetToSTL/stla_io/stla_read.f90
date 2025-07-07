subroutine stla_read ( input_file_name, node_num, face_num, node_xyz, &
  face_node, face_normal, ierror )

!*******************************************************************************
!
!! STLA_READ reads graphics information from an ASCII StereoLithography file.
!
!  Modified:
!
!    22 September 2005
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    3D Systems, Inc,
!    Stereolithography Interface Specification,
!    October 1989.
!
!  Parameters:
!
!    Input, character ( len = * ) INPUT_FILE_NAME, the name of the input file.
!
!    Input, integer NODE_NUM, the number of vertices defined.
!
!    Input, integer FACE_NUM, the number of faces defined.
!
!    Output, real ( kind = 8 ) NODE_XYZ(3,NODE_NUM), the coordinates of points.
!
!    Output, integer FACE_NODE(3,FACE_NUM), the nodes that make up each face.
!
!    Output, real ( kind = 8 ) FACE_NORMAL(3,FACE_NUM), the normal vector
!    at each face.
!
  implicit none

  integer face_num
  integer node_num

  logical done
  real ( kind = 8 ) dval
  integer face
  integer face_node(3,face_num)
  real ( kind = 8 ) face_normal(3,face_num)
  integer i
  integer ierror
  character ( len = * ) input_file_name
  integer ios
  integer iunit
  integer lchar
  integer node
  real ( kind = 8 ) node_xyz(3,node_num)
  logical s_eqi
  integer state
  real ( kind = 8 ) temp(3)
  character ( len = 256 ) text
  integer text_num
  integer vertex
  character ( len = 256 ) word1
  character ( len = 256 ) word2

  ierror = 0
  state = 0
  text_num = 0
  face = 0
  node = 0
!
!  Open the file.
!
  call get_unit ( iunit )

  open ( unit = iunit, file = input_file_name, status = 'old', iostat = ios )

  if ( ios /= 0 ) then
    write ( *, '(a)' ) ' '
    write ( *, '(a)' ) 'STLA_READ - Fatal error!'
    write ( *, '(a)' ) '  Could not open the file "' // &
      trim ( input_file_name ) // '".'
    ierror = 1
    return
  end if
!
!  Read the next line of text.
!
  do

    read ( iunit, '(a)', iostat = ios ) text

    if ( ios /= 0 ) then
      if ( state /= 0 .and. state /= 1 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning.'
        write ( *, '(a)' ) '  End-of-file, but model not finished.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if
      exit
    end if

    text_num = text_num + 1
    done = .true.
!
!  Read the first word in the line.
!
    call word_next_read ( text, word1, done )
!
!  "Doctor" the text, changing a beginning occurrence of:
!
!      END FACET to ENDFACET
!      END LOOP to ENDLOOP
!      END SOLID to ENDSOLID
!      FACET NORMAL to FACETNORMAL
!      OUTER LOOP to OUTERLOOP
!
    if ( s_eqi ( word1, 'END' ) .or. &
         s_eqi ( word1, 'FACET' ) .or. &
         s_eqi ( word1, 'OUTER' ) ) then

      call word_next_read ( text, word2, done )
      call s_cat ( word1, word2, word1 )

    end if
!
!  This first word tells us what to do.
!
!  SOLID - begin a new solid.
!    Valid in state 0, moves to state 1.
!  ENDSOLID - end current solid.
!    Valid in state 1, moves to state 0.
!
!  FACETNORMAL - begin a new facet.
!    Valid in state 0 or 1, moves to state 2.
!  ENDFACET - end current facet.
!    Valid in state 2, moves to state 1.
!
!  OUTERLOOP - begin a list of vertices.
!    Valid in state 2, moves to state 3, sets vertex count to 0.
!  ENDLOOP - end vertex list.
!    Valid in state 3, moves to state 2.
!
!  VERTEX - give coordinates of next vertex.
!    Valid in state 3.
!
!  End of file -
!    Valid in state 0 or 1.
!
    if ( s_eqi ( word1, 'SOLID' ) ) then

      if ( state /= 0 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for SOLID.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 1

    else if ( s_eqi ( word1, 'ENDSOLID' ) ) then

      if ( state /= 1 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for ENDSOLID.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 0

    else if ( s_eqi ( word1, 'FACETNORMAL' ) ) then

      if ( state /= 0 .and. state /= 1 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for FACET.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 2
      face = face + 1

      if ( face_num < face ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  More faces being read than expected.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      do i = 1, 3
        face_normal(i,face) = 0.0D+00
        call word_next_read ( text, word2, done )
        if ( .not. done ) then
          call s_to_d ( word2, dval, ierror, lchar )
          if ( ierror == 0 ) then
            face_normal(i,face) = dval
          end if
        end if
      end do

    else if ( s_eqi ( word1, 'ENDFACET' ) ) then

      if ( state /= 2 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for ENDFACET.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 1

    else if ( s_eqi ( word1, 'OUTERLOOP' ) ) then

      if ( state /= 2 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for OUTERLOOP.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 3
      vertex = 0

    else if ( s_eqi ( word1, 'ENDLOOP' ) ) then

      if ( state /= 3 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for ENDLOOP.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      state = 2

    else if ( s_eqi ( word1, 'VERTEX' ) ) then

      if ( state /= 3 ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Model not in right state for VERTEX.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      if ( 3 <= vertex ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  Too many vertices for face.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      do i = 1, 3
        call word_next_read ( text, word2, done )
        call s_to_d ( word2, dval, ierror, lchar )
        temp(i) = dval
      end do

      if ( node_num <= node ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_READ - Warning!'
        write ( *, '(a)' ) '  More nodes being read than expected.'
        write ( *, '(a,i6)' ) '  File line number = ', text_num
        ierror = 1
        return
      end if

      node = node + 1
      node_xyz(1:3,node) = temp(1:3)

      vertex = vertex + 1
      face_node(vertex,face) = node

    else

      write ( *, '(a)' ) ' '
      write ( *, '(a)' ) 'STLA_READ - Warning!'
      write ( *, '(a)' ) '  Unrecognized line in file.'
      write ( *, '(a,i6)' ) '  File line number = ', text_num
      ierror = 1
      return

    end if

  end do

  return
end
