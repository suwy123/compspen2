(set-logic QF_SLAH)

(declare-datatypes (
	(data_t 0)
	) (
	((c_data_t (sz Int)))
	)
)


(declare-heap (Int data_t) 
)

;; heap chunk
(define-fun hck ((?x Int) (?y Int)) Bool
	(and
		(>= (- ?y ?x) 2)
		(sep
			(pto ?x (c_data_t (- ?y ?x)))
			(blk (+ ?x 1) ?y)
		)
	)
)
;; heap list
(define-fun-rec hls ((?x Int) (?y Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int))
			(sep
				(hck ?x ?w)
				(hls ?w ?y)
			)
		)
	)
)


;; (sz > 256 &
;; !(a = 0) &
;; !(sz = 0) &
;; !(sz = 1) :
;; pto(a, sz) *
;; blk(a+1,a+sz)
;; )
;; ==>
;; hls(a, a+sz)


;; declare variables
(declare-const a Int)
(declare-const sz Int)


(assert
	(and
		(< 256 sz)
		(distinct a 0)
		(distinct sz 0)
		(distinct sz 1)
		(sep
			(pto a (c_data_t sz))
			(blk (+ a 1) (+ a sz))
		)
	)
)

(assert
	(not
		(hls a (+ a sz))
	)
)

(check-sat)
;;0.38s unsat:the entl is true
