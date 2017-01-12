 (use-modules (opencog)
              (opencog nlp aiml)
              (opencog persist-sql))

(primitive-load "../tests/testCase2/predicates.scm")
(sql-open "mycogdata" "opencog_user" "cheese")
(sql-store)
(sql-close)
