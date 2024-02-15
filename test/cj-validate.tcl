#!/usr/bin/env tclsh

source [file join [file dirname [info script]] "test.tcl"]

if {$argc != 2} {
  puts "ERROR: incorrect number of arguments"
  exit 1
}
set cjValidatePath [lindex $argv 0]
set projectPath [lindex $argv 1]

foreach testFile [glob-r [file join $projectPath "data"] *.json] {
  testCmd [list $cjValidatePath --csp $testFile] -exit-eq 0 -out-eq "Valid"
}

foreach testFile [glob-r [file join $projectPath "test" "data" "validation-neg"] *.json] {
  testCmd [list $cjValidatePath --csp $testFile] -exit-ne 0 -out-eq "Invalid"
}
