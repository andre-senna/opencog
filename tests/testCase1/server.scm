(use-modules (opencog)
             (opencog cogserver)
             (opencog logger))

(define (function-1 ticket)
    (cog-logger-info "CALLED function-1 ~s" ticket)
    (string-append "return-1-" ticket)
)

(define (function-2 ticket)
    (cog-logger-info "CALLED function-2 ~s" ticket)
    (string-append "return-2-" ticket)
)

(cog-logger-set-level! "debug")
(cog-logger-set-filename! "/tmp/cogserver.log")
(cog-logger-set-stdout! #t)

(start-cogserver "../tests/testCase1/custom_cogserver.conf")
(start-cogserver)
