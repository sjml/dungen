sir("testingInProgress", 0)
push("_testing.UnitTest")

if gir("testingInProgress") ~= 0 then
    quit("ERROR: Unit tests did not complete", 1)
end
