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



;; 256 <= oldsz &
;; y = hst + oldsz &
;; t = hst + oldsz
;; :
;; integer(hst,oldsz) *
;; blk(hst+1,y) *
;; hls(y,hen)
;; 
;; ==>
;; 
;; hck(hst, oldsz, t) *
;; hls(t, hen)

;; declare variables
(declare-const hst Int)
(declare-const hen Int)

(declare-const x0 Int)
(declare-const sz0 Int)
(declare-const y Int)
(declare-const t Int)

(assert (and
		(< 256 sz0)
		(= hst x0)
		(= y (+ x0 sz0))
		(= t (+ x0 sz0))
		(sep
			(pto x0 (c_data_t sz0))
			(blk (+ x0 1) y)
			(hls y hen)
		)
	)
)

(assert (not
		(sep
			(pto hst (c_data_t sz0))
			(blk (+ hst 1) t)
			(hls y hen)
		)
	)
)

(check-sat)
;; 0.78s unsat : entl is true
