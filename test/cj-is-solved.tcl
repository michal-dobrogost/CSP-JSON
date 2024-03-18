#!/usr/bin/env tclsh

source [file join [file dirname [info script]] "test.tcl"]

if {$argc != 2} {
  puts "ERROR: incorrect number of arguments"
  exit 1
}
set cjIsSolvedPath [lindex $argv 0]
set projectPath [lindex $argv 1]

set filePath [file join $projectPath "data" "human" "color-australia.json"]

# correct soluiton
testCmd [list $cjIsSolvedPath --csp $filePath --solution {[0,1,2,0,1,0,0]}] -exit-eq 0 -out-eq "true"

# invalid solution
testCmd [list $cjIsSolvedPath --csp $filePath --solution {[0,0,0,0,0,0,0]}] -exit-eq 0 -out-eq "false"

# solution too short
testCmd [list $cjIsSolvedPath --csp $filePath --solution {[0,0,0,0,0,0]}] -exit-eq 1 -out-eq ""

# soluiton too long
testCmd [list $cjIsSolvedPath --csp $filePath --solution {[0,0,0,0,0,0,0,0]}] -exit-eq 1 -out-eq ""

# --csp is not a file
testCmd [list $cjIsSolvedPath --csp $projectPath --solution {}] -exit-eq 1 -out-eq ""
