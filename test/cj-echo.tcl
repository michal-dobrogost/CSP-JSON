#!/usr/bin/env tclsh

source [file join [file dirname [info script]] "test.tcl"]

if {$argc != 2} {
  puts "ERROR: incorrect number of arguments"
  exit 1
}
set cjEchoPath [lindex $argv 0]
set projectPath [lindex $argv 1]

foreach testFile [glob-r [file join $projectPath "data"] *.json] {
  set fp [open $testFile r]
  set expected [read -nonewline $fp]
  close $fp
  testCmd [list $cjEchoPath --csp $testFile] -exit-eq 0 -out-eq $expected
}
