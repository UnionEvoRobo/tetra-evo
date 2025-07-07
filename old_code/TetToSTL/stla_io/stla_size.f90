subroutine stla_size ( input_file_name, solid_num, node_num, face_num )

!*******************************************************************************
!
!! STLA_SIZE determines sizes associated with an STLA file.
!
!  Discussion:
!
!    This routine assumes that the file is a legal STLA file.
!
!    To perform checks on the file, call STLA_CHECK first.
!
!    Note that the counts for the number of nodes and edges are
!    overestimates, since presumably, most nodes will be defined several
!    times, once for each face they are part of, and most edges will
!    be defined twice.
!
!  Modified:
!
!    23 September 2005
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
!    Output, integer SOLID_NUM, the number of solids defined.
!    Presumably, this is 1.
!
!    Output, integer NODE_NUM, the number of vertices defined.
!
!    Output, integer FACE_NUM, the number of faces defined.
!
  implicit none

  logical check
  logical done
  real ( kind = 8 ) dval
  integer face_num
  integer i
  integer ierror
  character ( len = * ) input_file_name
  integer ios
  integer iunit
  integer lchar
  integer node_num
  logical s_eqi
  integer solid_num
  integer state
  character ( len = 256 ) text
  integer text_num
  integer vertex
  character ( len = 256 ) word1
  character ( len = 256 ) word2

  ierror = 0

  state = 0
  text_num = 0

  solid_num = 0
  node_num = 0
  face_num = 0
!
!  Open the file.
!
  call get_unit ( iunit )

  open ( unit = iunit, file = input_file_name, status = 'old', iostat = ios )

  if ( ios /= 0 ) then
    write ( *, '(a)' ) ' '
    write ( *, '(a)' ) 'STLA_SIZE - Fatal error!'
    write ( *, '(a)' ) '  Could not open the file "' &
      // trim ( input_file_name ) // '".'
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

    if ( done ) then
      return
    end if
!
!  "Doctor" the text, changing a beginning occurrence of:
!
!      END FACET to ENDFACET
!      END LOOP to ENDLOOP
!      END SOLID to ENDSOLID
!      FACET NORMAL to FACETNORMAL
!      OUTER LOOP to OUTERLOOP
!
    if ( s_eqi ( word1, 'END' ) ) then

      call word_next_read ( text, word2, done )

      if ( .not. s_eqi ( word2, 'FACET' ) .and. &
           .not. s_eqi ( word2, 'LOOP' ) .and. &
           .not. s_eqi ( word2, 'SOLID' ) ) then
        return
      end if

      call s_cat ( word1, word2, word1 )

    else if ( s_eqi ( word1, 'FACET' ) ) then

      call word_next_read ( text, word2, done )

      if ( .not. s_eqi ( word2, 'NORMAL' ) ) then
        return
      end if

      call s_cat ( word1, word2, word1 )

    else if ( s_eqi ( word1, 'OUTER' ) ) then

      call word_next_read ( text, word2, done )

      if ( .not. s_eqi ( word2, 'LOOP' ) ) then
        return
      end if

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
!    Valid in state 2, moves to state 3.
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
        return
      end if

      state = 1

    else if ( s_eqi ( word1, 'ENDSOLID' ) ) then

      if ( state /= 1 ) then
        return
      end if

      state = 0

      solid_num = solid_num + 1

    else if ( s_eqi ( word1, 'FACETNORMAL' ) ) then

      if ( state /= 0 .and. state /= 1 ) then
        return
      end if

      state = 2

      do i = 1, 3

        call word_next_read ( text, word2, done )

        if ( done ) then
          return
        end if

        call s_to_d ( word2, dval, ierror, lchar )

        if ( ierror /= 0 ) then
          return
        end if

      end do

    else if ( s_eqi ( word1, 'ENDFACET' ) ) then

      if ( state /= 2 ) then
        return
      end if

      state = 1

      face_num = face_num + 1

    else if ( s_eqi ( word1, 'OUTERLOOP' ) ) then

      if ( state /= 2 ) then
        return
      end if

      state = 3
      vertex = 0

    else if ( s_eqi ( word1, 'ENDLOOP' ) ) then

      if ( state /= 3 ) then
        return
      end if

      state = 2

    else if ( s_eqi ( word1, 'VERTEX' ) ) then

      if ( state /= 3 ) then
        return
      end if

      if ( 3 <= vertex ) then
        write ( *, '(a)' ) ' '
        write ( *, '(a)' ) 'STLA_SIZE - Fatal error!'
        write ( *, '(a)' ) '  Too many vertices for a face.'
        ierror = 1
        return
      end if

      do i = 1, 3

        call word_next_read ( text, word2, done )

        if ( done ) then
          return
        end if

        call s_to_d ( word2, dval, ierror, lchar )

        if ( ierror /= 0 ) then
          return
        end if

      end do

      vertex = vertex + 1
      node_num = node_num + 1

    else

      return

    end if

  end do
!
!  Close the file.
!
  close ( unit = iunit )

  return
end
