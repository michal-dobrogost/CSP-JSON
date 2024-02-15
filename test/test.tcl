# Runs an external command and tests the exit status and stdout against expectations.
#
# @arg modeExit is one of the tcl equality strings {-exit-eq -exit-ne} and is used.
# @arg expectedExit is the expected integral exit status of the command.
# @arg modeOut can be one of {-out-eq -out-ends-with} and controls how to matched expectedOut.
# @arg expectedOut is the expected stdout of the command.
proc testCmd {cmd modeExit expectedExit modeOut expectedOut} {
  puts -nonewline "$cmd ... "

  set resultExit [catch {exec -ignorestderr {*}$cmd} msg]
  if {$resultExit} {
    # Remove error message appended by exec
    set msg [join [lrange [split $msg "\n"] 0 end-1] "\n"]
  }
  if {$modeExit eq "-exit-eq"} {
    if {$resultExit != $expectedExit} {
      puts "ERROR: Command exited with $resultExit != expected exit $expectedExit while outputing: $msg"
      exit 1
    }
  } elseif {$modeExit eq "-exit-ne"} {
    if {$resultExit == $expectedExit} {
      puts "ERROR: Command exited with $resultExit != expected exit $expectedExit while outputing: $msg"
      exit 1
    }
  } else {
    puts "ERROR: unknown modeExit: $modeExit"
    exit 1
  }

  if {$modeOut eq "-out-ends-with"} {
    set expectedLen [llength [split $expectedOut "\n"]]
    set result [join [lrange [split $msg "\n"] end-[expr {$expectedLen-1}] end] "\n"]
  } elseif {$modeOut eq "-out-eq"} {
    set result $msg
  } else {
    puts "ERROR: Unknown modeOut: $modeOut"
    exit 1
  }

  if {! [string equal $result $expectedOut]} {
    puts "FAIL"
    puts "expected $modeOut:\n|$expectedOut|"
    puts "got:\n|$result|"
    puts "done."
    exit 1
  }

  puts "OK"
}

# A recursive glob that visits all matching files under a path.
# eg. [glob-r [file join "/home" "user"] *.txt]
proc glob-r {{dir .} args} {
    set res {}
    foreach i [lsort [glob -nocomplain -dir $dir *]] {
        if {[file isdirectory $i]} {
            eval [list lappend res] [eval [linsert $args 0 glob-r $i]]
        } else {
            if {[llength $args]} {
                foreach arg $args {
                    if {[string match $arg $i]} {
                        lappend res $i
                        break
                    }
                }
            } else {
                lappend res $i
            }
        }
    }
    return $res
}
