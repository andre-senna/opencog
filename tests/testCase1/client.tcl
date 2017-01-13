#!/usr/bin/tclsh

# Delay in msecs
# Wait DELAY msecs after sending a command to cogserver before trying to read the asnwer
set DELAY 1000

set NOANSWER "NO ANSWER"

if {[llength $argv] != 2} {
    puts "Usage: $argv0 <instance id> <cycles>"
    exit
}

set instance [lindex $argv 0]
set N [lindex $argv 1]

proc getCurrentTime {} {
    return [format "\[%s\]" [clock format [expr [clock seconds]] -format "%Y-%m-%d %H:%M:%S"]]
}

proc log {str} {
    puts [format "%s %s" [getCurrentTime] $str]
}

proc command {cmd} {

    global sk NOANSWER DELAY

    log $cmd
    puts $sk $cmd
    flush $sk
    after $DELAY
    set cmdAnswer ""
    gets $sk cmdAnswer
    log "answer: <$cmdAnswer>"
    if {[string compare $cmdAnswer ""] == 0} {
        set cmdAnswer $NOANSWER
        log "answer: <$cmdAnswer>"
    }
    return $cmdAnswer
}

# "main"

set sk [socket 172.17.0.4 17001]
# Configure sk to make "gets" non-blocking
fconfigure $sk -blocking 0
command "(function-1 \"startup\")"
after $DELAY
gets $sk cmdAnswer
log "answer: <$cmdAnswer>"

set errorCount 0
for {set i 0} {$i < $N} {incr i} {
    set ticket "TICKET-$instance-$i"
    set answer [command "(function-1 \"$ticket\")"]
    if {[string compare $answer "return-1-$ticket"] != 0} {
        log "ERROR - <$answer> != <return-1-$ticket>"
        incr errorCount
    }
    set answer [command "(function-2 \"$ticket\")"]
    if {[string compare $answer "return-2-$ticket"] != 0} {
        log "ERROR - <$answer> != <return-2-$ticket>"
        set flag 1
        incr errorCount
    }
}
log "errorCount = $errorCount"
