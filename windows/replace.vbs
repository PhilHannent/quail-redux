Const ForReading = 1
Const ForWriting = 2
Const TristateTrue = -1
Const TristateUseDefault = -2

strFileName = Wscript.Arguments(0)
strOldText = Wscript.Arguments(1)
strNewText = Wscript.Arguments(2)
' The file will come in using Unicode so we cannot load it using the FSO's default mode as
' That turns out to just read ASCII
Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objFile = objFSO.OpenTextFile(strFileName, ForReading, False, TristateUseDefault)
strText = objFile.ReadAll
objFile.Close

sResult = Replace(strText, strOldText, strNewText)

Set objFile = objFSO.OpenTextFile(strFileName, ForWriting)
objFile.Write(sResult)
objFile.Close