Option Explicit
'- - - - - - - - - - - - - - -
' TS3_DSP_Timestamp.vbs -- automatically edit the Timestamp.c file
'                       insert current date and time info.
'                       In the future we may expand this to also
'                       update a "version" field.  For now we bump the ls
'                       element of the version by +1.
'
'8/29/2017 modify it to work like FPGA timestamp macro -- display version and old time stamp
'          ask user if they want to update it, allow manual editing of version.
'
'8/28/2017 Special mod for Bootloader projects, ls byte of Rev3 timestamps is 
' 0F-> Bootloader// 00->Dsp Main// 01, 02, 03-> FPGA
'- - - - - - - - - - - - - - -


call edit_timestamp_c

'- - - - - - - - - - - - - - -
'
Function hex_from_char_in_string(character_string)
'
' Called with a character string
' Returns a string of 2-digit hex numbers representing characters in string

  Dim i, length, c, h

  while i < Len(character_string)
    i = i + 1
    c = Mid(character_string,i,1)
    h = Hex(Asc(c))
    if Len(h) < 2 Then h = "0" & h
    hex_from_char_in_string = hex_from_char_in_string & "0x" & h & " "
  wend


End Function
'
'- - - - - - - - - - - - - - - -
'
Function ascii_hex16_from_2_twoDigit_ascii_decimals(twoDigit_ascii_decimal_1,twoDigit_ascii_decimal_2)
' Given 2 2-digit ascii decimals, like "01", "12"
' return an ascii hex like "0x010C"
' OK, so we also allow 3-digit ascii decimal inputs (!!!)
  Dim temp_str, x
  Dim temp_twoDigit_ascii_decimal
  
  'make sure we are working w/ 3 characters, add leading 0's if necessary
  temp_twoDigit_ascii_decimal = twoDigit_ascii_decimal_1
  If (len(temp_twoDigit_ascii_decimal) < 1) then 
    temp_twoDigit_ascii_decimal = "000"  
  ElseIf (len(temp_twoDigit_ascii_decimal) < 2) Then
    temp_twoDigit_ascii_decimal = "00" & temp_twoDigit_ascii_decimal
  ElseIf (len(temp_twoDigit_ascii_decimal) < 3) Then
    temp_twoDigit_ascii_decimal = "0" & temp_twoDigit_ascii_decimal
  End If
  
  temp_str = "0x"
  x = (asc(mid(temp_twoDigit_ascii_decimal,1,1)) - &H30&) * 10
  x =  x + (asc(mid(temp_twoDigit_ascii_decimal,2,1)) - &H30) * 10
  x =  x + (asc(mid(temp_twoDigit_ascii_decimal,3,1)) - &H30)
  temp_str = temp_str & Right(("00" & Hex(x)),2)

  'make sure we are working w/ 3 characters, add leading 0's if necessary
  temp_twoDigit_ascii_decimal = twoDigit_ascii_decimal_2
  If (len(temp_twoDigit_ascii_decimal) < 1) then 
    temp_twoDigit_ascii_decimal = "000"  
  ElseIf (len(temp_twoDigit_ascii_decimal) < 2) Then
    temp_twoDigit_ascii_decimal = "00" & temp_twoDigit_ascii_decimal
  ElseIf (len(temp_twoDigit_ascii_decimal) < 3) Then
    temp_twoDigit_ascii_decimal = "0" & temp_twoDigit_ascii_decimal
  End If

  x = (asc(mid(temp_twoDigit_ascii_decimal,1,1)) - &H30&) * 10
  x =  (x + (asc(mid(temp_twoDigit_ascii_decimal,2,1)) - &H30)) * 10
  x =  x + (asc(mid(temp_twoDigit_ascii_decimal,3,1)) - &H30)
  temp_str = temp_str & Right(("00" & Hex(x)),2)
 
 ascii_hex16_from_2_twoDigit_ascii_decimals = temp_str
 End Function
'
'- - - - - - - - - - - - - - -
'
Function mm_dd_yyyy_from_FormatDate(Fmt_Date)
'
' Called with a string containing a date like 12/23/52 or 1/1/2001
' Returns a formated string of the data as mm/dd/yyyy
' Always 2-digit month, day, 4-digit year 
'
  Dim mm
  Dim dd
  Dim yy
  Dim s_Date
  s_Date = Fmt_Date
  mm = Mid(s_Date,1,InStr(1,s_Date,"/") - 1)
  s_Date = Mid(s_Date,InStr(1,s_Date,"/") + 1)
  dd = Mid(s_Date,1,InStr(1,s_Date,"/") - 1)
  yy = Mid(s_Date,InStr(1,s_Date,"/") + 1)
'  if (Len(mm) < 2) then mm = "0" & mm 
'  if (Len(dd) < 2) then dd = "0" & dd 
'  if (Len(yy) < 2) then yy = "0" & yy 
'  if (Len(yy) > 2) then yy = Right(yy,2)
  mm_dd_yyyy_from_FormatDate = Right(("00" & mm),2) & "/" _
                             & Right(("00" & dd),2) & "/" _
							 & Right(("0000" & yy),4)
End Function

'- - - - - - - - - - - - - - -
'
Function hr_mn_from_FormatDate(s_Time)
'
' Called with a string containing a time like 12:18 or 1:05
' Returns a 4-character hrmn format of the time.
' Used in creating a fixed-length timestamp. 
'
  Dim hr
  Dim mn

  hr = Mid(s_Time,1,InStr(1,s_Time,":") - 1)
  mn = Mid(s_Time,InStr(1,s_Time,":") + 1)
  if (Len(hr) < 2) then hr = "0" & hr 
  if (Len(mn) < 2) then mn = "0" & mn 
  hr_mn_from_FormatDate = hr & ":" & mn

End Function
'
'- - - - - - - - - - - - - - -
'
Function yymmdd_from_FormatDate(Fmt_Date)
'
' Called with a string containing a date like 12/23/52 or 1/1/2001
' Returns a 6-character yymmdd format of the date.
' Used in creating a filename like fname_yymmdd 
'
  Dim mm
  Dim dd
  Dim yy
  Dim s_Date
  s_Date = Fmt_Date
  mm = Mid(s_Date,1,InStr(1,s_Date,"/") - 1)
  s_Date = Mid(s_Date,InStr(1,s_Date,"/") + 1)
  dd = Mid(s_Date,1,InStr(1,s_Date,"/") - 1)
  yy = Mid(s_Date,InStr(1,s_Date,"/") + 1)
  if (Len(mm) < 2) then mm = "0" & mm 
  if (Len(dd) < 2) then dd = "0" & dd 
  if (Len(yy) < 2) then yy = "0" & yy 
  if (Len(yy) > 2) then yy = Right(yy,2)
  yymmdd_from_FormatDate = yy & mm & dd
End Function

'- - - - - - - - - - - - - - -
'
Function hrmn_from_FormatDate(s_Time)
'
' Called with a string containing a time like 12:18 or 1:05
' Returns a 4-character hrmn format of the time.
' Used in creating a fixed-length timestamp. 
'
  Dim hr
  Dim mn

  hr = Mid(s_Time,1,InStr(1,s_Time,":") - 1)
  mn = Mid(s_Time,InStr(1,s_Time,":") + 1)
  if (Len(hr) < 2) then hr = "0" & hr 
  if (Len(mn) < 2) then mn = "0" & mn 
  hrmn_from_FormatDate = hr & mn

End Function
'- - - - - - - - - - - - - - -
'
Function fix_prompt_string(prompt_string) 
'Insert a " " after each "\" so that long path names are wrapped,
' instead of being truncated
  Dim Str_1
  Dim Str_2
  Dim I

  Str_1 = prompt_string
  I = 1
  Do While (I > 0)
    I = InStr(I,str_1,"\")
    if (I > 0) Then
      str_2 = Mid(str_1,1,I) & " " & Mid(str_1,I+1)
      str_1 = str_2
      I = I + 1
    end if
  Loop

  fix_prompt_string = str_1
End Function
'- - - - - - - - - - - - - - -
'
Function Parse_Numeric_Info_From_Rev_String(Rev_String)
' There's got to be an easier way than this.
' Here we extract the next numeric info from Rev_String and return it,
' having also discarded leading characters from Rev_String

  Dim Num_String
  
  'Discard non-numeric lead characters 1 at a time
  While ((Len(Rev_String) > 0) And (Not IsNumeric(Mid(Rev_String,1,1))))
    Rev_String = Mid(Rev_String,2)
  Wend
  'Extract Numeric Characters
  Num_String = ""
  While ((Len(Rev_String) > 0) And (IsNumeric(Mid(Rev_String,1,1))))
    Num_String = Num_String & Mid(Rev_String,1,1)
	Rev_String = Mid(Rev_String,2)
  Wend
  Parse_Numeric_Info_From_Rev_String = Num_String
End Function
'- - - - - - - - - - - - - - -
'
Function Does_User_Want_to_Change_Revision(Rev1, Rev2, Rev3, Rev4, Original_Rev_String)

  Dim Rev_String
  Dim prompt_String
  
  Rev_String = "Rev: " & Rev1 & "." & Rev2 & "." & Rev3 
  Original_Rev_String = Rev_String

  prompt_string = "Want to update "
  If (Rev4 = "00") Then
     prompt_string = prompt_string & "DSP MAIN"
  ElseIf (Rev4 = "0F") Then
     prompt_string = prompt_string & "DSP BOOTLOADER"
  Else
     prompt_string = prompt_string & "BAD Rev4"
  End If
  
  prompt_string = prompt_string & _  
                " Revision Info? " & Chr(13) & Chr(10)_
                & " " & Chr(13) & Chr(10)_
                & "  or CANCEL to exit." & Chr(13) & Chr(10)_
                & " " 

  Rev_String = InputBox(prompt_string, "TS3_DSP_Timestamp",_
                 Rev_String)
  'If operator clicks CANCEL, InputBox returns zero length string ("")
  if (Rev_String = "") Then WScript.Quit 'operator CANCELED script
 
  ' Parse numerical revision info out of rev_string
  Rev1 = Parse_Numeric_Info_From_Rev_String(Rev_String)
  Rev2 = Parse_Numeric_Info_From_Rev_String(Rev_String)
  Rev3 = Parse_Numeric_Info_From_Rev_String(Rev_String)

  Does_User_Want_to_Change_Revision = True
End Function
'- - - - - - - - - - - - - - -
'
Sub Insert_Timestamp(modify_this_src_file)
'
'Read the given file, looking for 
' const char *version = "V01.01.001";
' const char *ts_dateStamp = "01/01/2014\0";
' const char *ts_timeStamp = "09:41\0";

'Replace the following datestamp info with current "mm/dd/yyyy"
'Replace the following timeestamp info with current "Hr:Mm"
'Increment the LS digit of version"

'Then look for
' const Uint16 timeStamp_t1 = 0x0000; // BCD YYMM
' const Uint16 timeStamp_t2 = 0x0000; // BCD DDHr
' const Uint16 timeStamp_t3 = 0x0000; // BCD MnA5
' const Uint16 revision_rv1 = 0x0000; // byte-Rev1 . byte-Rev2
' const Uint16 revision_rv2 = 0x0000; // . byte-Rev3 . byte-device(0 for DSP, 1 for FPGA 1, etc)

'take the same info saved as const char*'s above and format it in const Uint16's in
'the same format used for FPGA timestamps

  Dim contents_of_file, copy_of_contents
  Dim fso, f, Msg
  Const ForReading = 1, ForWriting = 2, ForAppending = 8
  Dim mm_dd_yyyy, hr_mn, i, j, k, m, n, p, x
  Dim rewrite
  Dim old_ver3, new_ver3, old_ver1, old_ver2, old_ver4, whole_new_revision, new_rev
  Dim yymmdd, hrmn, rev1_2, rev3_2, rev3_0F
  Dim yes_change_revision
  Dim Original_Rev_String

  'Don't rewrite unless we find a timestamp or version and update it
  rewrite = False

  'Read entire contents of Source file into contents_of_file
  Set fso = CreateObject("Scripting.FileSystemObject")
  Set f = fso.OpenTextFile(modify_this_src_file, ForReading)
  contents_of_file = f.ReadAll

  'Get current mm_dd_yyyy and hr_mn  -- for old-style ascii implementation
  mm_dd_yyyy = mm_dd_yyyy_from_FormatDate(FormatDateTime(Date))
  hr_mn = hr_mn_from_FormatDate(FormatDateTime(Time,4))

  'Get current yymmdd and hrmn  
  ' Save this for when we do the new "FPGA" format implementation below.
  yymmdd = yymmdd_from_FormatDate(FormatDateTime(Date))
  hrmn = hrmn_from_FormatDate(FormatDateTime(Time,4))

'call MsgBox ("(FormatDateTime(Date) looks like this: " & (FormatDateTime(Date) ) ) 
'call MsgBox ("(FormatDateTime(Time,4) looks like this: " & (FormatDateTime(Time,4) )  )
'call MsgBox ("yymmdd_from_FormatDate(FormatDateTime(Date)) looks like this: " & yymmdd ) 
'call MsgBox ("hrmn_from_FormatDate(FormatDateTime(Time,4)) looks like this: " & hrmn ) 

  ' Search and for Version info
  i = InStr(1,contents_of_file,"const char *version = """)
  if i > 0 then
    j = InStr(i,contents_of_file,"""")   ' locate first quotation mark (")
    p = InStr(j+1,contents_of_file,"V")  ' locate "V" 
    k = InStr(j+1,contents_of_file,"""") ' locate 2nd quotation mark (")
	m = InStr(j+1,contents_of_file,".")  ' locate first dot (.)
	n = InStr(m+1,contents_of_file,".")  ' locate 2nd dot (.)
    if (j-i > 30) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) _
	   Or (m-j > 5) Or (m-j < 1) Or (n-m > 5) Or (n-m < 1) Or (p-j <> 1) Then
	  call MsgBox ("Problem parsing ""const char *version"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    old_ver3 = Mid(contents_of_file,n+1,(k-n)-3)
	'call MsgBox (old_ver3)
  end if

  old_ver1 = Mid(contents_of_file,p+1,(m-p)-1)
  old_ver2 = Mid(contents_of_file,m+1,(n-m)-1)

  ' Search and for Version info
  i = InStr(1,contents_of_file,"Uint16 revision_rv2 = ")
  if i > 0 then
    j = InStr(i,contents_of_file,"0x")   ' locate first "0x"
    if (j-i > 30) Or (j-i < 1) Then
	  call MsgBox ("Problem parsing ""Uint16 revision_rv2"" line in Timestamp.c. Aborting." )  
      'call MsgBox ("i : " & i & " j : " & j)
      WScript.Quit 'Temporary End
    End If	  
    old_ver4 = Mid(contents_of_file,j+4,2) '2 hex digita indicating DSP / Bootloader / FPGA#
	'call MsgBox (old_ver4)
  end if

  yes_change_revision = Does_User_Want_to_Change_Revision(old_ver1, old_ver2, old_ver3, old_ver4, Original_Rev_String)  
  ' note: Does_User_Want_to_Change_Revision() returns True or exits without returning
  
  'Force/Limit revisions to 2 digits  
  old_ver1 = Right(("00" & old_ver1),2)
  old_ver2 = Right(("00" & old_ver2),2)
  old_ver3 = Right(("00" & old_ver3),2)
  old_ver4 = Right(("00" & old_ver4),2)

 'call MsgBox ("old_ver1: " & old_ver1 & " old_ver2: " & old_ver2 & " old_ver3: " & old_ver3 & " old_ver4: " & old_ver4)

  ' - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - - 
  ' Got this far Tuesday.  We display a message compatible with FPGA, asking do you want to change
  ' Revision?
  ' 1) make sure it works for Main program -- non bootloader
  ' 2) go ahead and use response to update revision and time stamps.  
  ' - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - - 
  'WScript.Quit 'Temporary End ********************************
	  
  ' Search and replace date stamp 
  i = InStr(1,contents_of_file,"const char *ts_dateStamp = """)
  if i > 0 then
    j = InStr(i,contents_of_file,"""") ' locate first quotation mark (")
    k = InStr(j+1,contents_of_file,"""") ' locate 2nd quotation mark (")
    if (j-i > 30) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const char *ts_dateStamp"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & mm_dd_yyyy _
       & "\0" _
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

  ' Search and replace time stamp 
   i = InStr(1,contents_of_file,"const char *ts_timeStamp = """)
  if i > 0 then
    j = InStr(i,contents_of_file,"""") ' locate first quotation mark (")
    k = InStr(j+1,contents_of_file,"""") ' locate 2nd quotation mark (")
    if (j-i > 30) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const char *ts_timeStamp"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & hr_mn _
       & "\0" _
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

  ' Search and Update Version 
   i = InStr(1,contents_of_file,"const char *version = """)
  if i > 0 then
    j = InStr(i,contents_of_file,"""")   ' locate first quotation mark (")
    p = InStr(j+1,contents_of_file,"V")  ' locate "V" 
    k = InStr(j+1,contents_of_file,"""") ' locate 2nd quotation mark (")
	m = InStr(j+1,contents_of_file,".")  ' locate first dot (.)
	n = InStr(m+1,contents_of_file,".")  ' locate 2nd dot (.)
    if (j-i > 30) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) _
	   Or (m-j > 5) Or (m-j < 1) Or (n-m > 5) Or (n-m < 1) Or (p-j <> 1) Then
	  call MsgBox ("Problem parsing ""const char *version"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True
    copy_of_contents = Mid(contents_of_file,1,p) _
       & old_ver1 & "." & old_ver2 & "." & old_ver3 _
       & "\0" _
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

  ' Save this for when we do the new "FPGA" format implementation below.
  rev1_2 = ascii_hex16_from_2_twoDigit_ascii_decimals(old_ver1,old_ver2)
  If (old_ver4 = "0F") Then
     rev3_2 = ascii_hex16_from_2_twoDigit_ascii_decimals(old_ver3,15)
  else	 
     rev3_2 = ascii_hex16_from_2_twoDigit_ascii_decimals(old_ver3,old_ver4)
  end if	 
  '0F (= 15 decimal)-> Bootloader// 00->Dsp Main// 01, 02, 03-> FPGA

  'Save new revision for completion message at end of script
  whole_new_revision = "V" & old_ver1 & "." & old_ver2 & "." & old_ver3
  
'call MsgBox ("old_ver1 looks like this: " & old_ver1 ) 
'call MsgBox ("old_ver2 looks like this: " & old_ver2 ) 
'call MsgBox ("new_ver3 looks like this: " & new_ver3 ) 
'call MsgBox ("rev1_2 looks like this: " & rev1_2 ) 
'call MsgBox ("rev3_2 looks like this: " & rev3_2 ) 

  ' Search and replace "timeStamp_t1" for FPGA-format timestamp/revision
   i = InStr(1,contents_of_file,"const Uint16 timeStamp_t1 = 0x")
   if i > 0 then
    j = InStr(i,contents_of_file,"=") ' locate equals sign (=)
    k = InStr(j+1,contents_of_file,";") ' locate semi-colon (;)
    if (j-i > 35) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const Uint16 timeStamp_t1"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & " " &  "0x" & mid(yymmdd,1,4)_
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

   ' Search and replace "timeStamp_t2" for FPGA-format timestamp/revision
   i = InStr(1,contents_of_file,"const Uint16 timeStamp_t2 = 0x")
   if i > 0 then
    j = InStr(i,contents_of_file,"=") ' locate equals sign (=)
    k = InStr(j+1,contents_of_file,";") ' locate semi-colon (;)
    if (j-i > 35) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const Uint16 timeStamp_t2"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & " " &  "0x" & mid(yymmdd,5,2) & mid(hrmn,1,2)_
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

   ' Search and replace "timeStamp_t3" for FPGA-format timestamp/revision
   i = InStr(1,contents_of_file,"const Uint16 timeStamp_t3 = 0x")
   if i > 0 then
    j = InStr(i,contents_of_file,"=") ' locate equals sign (=)
    k = InStr(j+1,contents_of_file,";") ' locate semi-colon (;)
    if (j-i > 35) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const Uint16 timeStamp_t3"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & " " &  "0x" & mid(hrmn,3,2) & "A5"_
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

   ' Search and replace "revision_rv1" for FPGA-format timestamp/revision
   i = InStr(1,contents_of_file,"const Uint16 revision_rv1 = 0x")
   if i > 0 then
    j = InStr(i,contents_of_file,"=") ' locate equals sign (=)
    k = InStr(j+1,contents_of_file,";") ' locate semi-colon (;)
    if (j-i > 35) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const Uint16 revision_rv1"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & " " & rev1_2 _
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

   ' Search and replace "revision_rv2" for FPGA-format timestamp/revision
   i = InStr(1,contents_of_file,"const Uint16 revision_rv2 = 0x")
   if i > 0 then
    j = InStr(i,contents_of_file,"=") ' locate equals sign (=)
    k = InStr(j+1,contents_of_file,";") ' locate semi-colon (;)
    if (j-i > 35) Or (j-i < 1) Or (k-j > 15) Or (k-j < 1) Or (k = 0) Or (j = 0) Then
	  call MsgBox ("Problem parsing ""const Uint16 revision_rv1"" line in Timestamp.c. Aborting." )  
      WScript.Quit 'Temporary End
    End If	  
    rewrite = True 
    copy_of_contents = Mid(contents_of_file,1,j) _
       & " " & rev3_2 _
     & Mid(contents_of_file,k)
    contents_of_file = copy_of_contents
  end if

  
  'Write it back out to the file
  if (rewrite) then
    Set f = fso.OpenTextFile(modify_this_src_file, ForWriting, True)
    f.Write contents_of_file
    Wscript.echo "TS3_DSP_Timestamped " & Chr(13) & Chr(10) _
	& Chr(13) & Chr(10) _
	&modify_this_src_file & Chr(13) & Chr(10) _
    & Chr(13) & Chr(10) _
	& mm_dd_yyyy & " " & hr_mn & Chr(13) & Chr(10) _
	& Chr(13) & Chr(10) _
	& whole_new_revision
    ' Following can be used to verify we are not adding or losing charcaters as
    '  we re-write the file
    'WScript.echo hex_from_char_in_string(Right(contents_of_file,20))
  end if
 
End Sub 
'- - - - - - - - - - - - - - -
'
Sub edit_timestamp_c()
'
'Find out what folder this VBS file resides in.
'See if Timestamp.c file exists.

  Dim Script_Folder
  Dim Timestamp_c_file_path
  Dim fso, f, fc, f1
  Dim prompt_string

  'start with the full path\file name of the script file
  Script_Folder = WScript.ScriptFullName
  Script_Folder = Mid(Script_Folder,1,InStrRev(Script_Folder,"\"))
  Timestamp_c_file_path = Script_Folder & "Timestamp.c"

  'Make sure Timestamp.c file exists
  Set fso = CreateObject("Scripting.FileSystemObject")

   If (Not fso.FileExists(Timestamp_c_file_path)) Then
        prompt_string = Timestamp_c_file_path
		prompt_string = fix_prompt_string(prompt_string)
        call MsgBox ("TS3_DSP_Timestamp.vbs: " & Chr(13) & Chr(10)_
         & "edit_timestamp_c()" & Chr(13) & Chr(10)_
         & "file does not exist : " & Chr(13) & Chr(10)_
		 & Chr(13) & Chr(10)_
         & prompt_string & Chr(13) & Chr(10))
		WScript.Quit
   end if

call Insert_Timestamp(Timestamp_c_file_path)

'End quietly

End Sub 
