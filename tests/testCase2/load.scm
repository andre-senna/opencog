 (use-modules (opencog)
              (opencog nlp aiml)
              (opencog persist-sql))

(sql-open "mycogdata" "opencog_user" "cheese")
(sql-load)
(sql-close)
