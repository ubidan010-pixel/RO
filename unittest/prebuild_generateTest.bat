set OUT="..\..\src\unittest\unittest.cpp"

python "..\..\extern\cxxtest\cxxtestgen.py" -o %OUT% --runner=ParenPrinter "..\..\src\unittest\*_Tests.h" --no-eh