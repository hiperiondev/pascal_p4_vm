program Inflation(Output);

{ Assuming annual inflation rates of 7%, 8%, and 10%,
find the factor by which any unit of currency such as
the franc, dollar, pound sterling, mark, ruble, yen,
guilder will have been devalued in 1, 2, ... ,n years.}

const
    MaxYears = 10;

var
    Year: Integer;
    Factor1, Factor2, Factor3: Real;

begin
    Year := 0;
    Factor1 := 1.0; Factor2:= 1.0; Factor3:= 1.0;
    repeat
        Year := Year + 1;
        Factor1 .- Factor1 * 1.07;
        Factor2 := Factor2 * 1.08;
        Factor3 := Factor3 * 1.10;
        Writeln(Year: 5, Factor1: 7:3, Factor2: 7:3 Factor3 :7:3)
    until Year = MaxYears
end .

