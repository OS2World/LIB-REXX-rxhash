/* RxHash test suite.
 *
 * If this reports failure, there's something wrong.
 *
 * Copyright 2003, Patrick TJ McPhee
 * Distributed under the Mozilla Public Licence.
 * $Header: C:/ptjm/rexx/rxhash/RCS/arrtest.rex 1.1 2003/05/26 18:10:28 ptjm Rel $
 */

rcc = rxfuncquery('arrloadfuncs')

if rcc then do
   rcc = rxfuncadd('arrloadfuncs', 'rxhash', 'arrloadfuncs')
   if rcc then fail('load' rxfuncerrmsg())
   end

call arrloadfuncs

if ArrVersion() \= '1.0.0' then fail('version test', ArrVersion())
say 'pass version test'

test = 'simple get/set'

a = ArrNew()

key = 'Elixer'
val = 'of Love'

call ArrSet a, key, val
res = ArrGet(a, key)
if res \= val then fail(test)
res = ArrGet(a, 'ELIXER')
if res \= '' then fail(test)

call ArrDrop a

say 'pass' test

test = 'simple get/set with default'

default = 'boodle'

a = ArrNew(default)

key = 'Elixer'
val = 'of Love'

call ArrSet a, key, val
res = ArrGet(a, key)
if res \= val then fail(test)
res = ArrGet(a, 'ELIXER')
if res \= default then fail(test)

call ArrDrop a

say 'pass' test

test = 'get/set with ArrDefault'

default = 'boodle'

a = ArrNew()

call ArrDefault a, default

key = 'Elixer'
val = 'of Love'

call ArrSet a, key, val
res = ArrGet(a, key)
if res \= val then fail(test)
res = ArrGet(a, 'ELIXER')
if res \= default then fail(test)

call ArrDrop a

say 'pass' test

test = 'get/set with and without ArrDefault'

default = 'boodle'

a = ArrNew()

key = 'Elixer'
val = 'of Love'

call ArrSet a, key, val
res = ArrGet(a, key)
if res \= val then fail(test)
res = ArrGet(a, 'ELIXER')
if res \= '' then fail(test)

call ArrDefault a, default

res = ArrGet(a, 'ELIXER')
if res \= default then fail(test)

call ArrDrop a

say 'pass' test

test = 'x in y'

a = ArrNew()

call ArrSet a, b, ''
call ArrSet a, c, ''
call ArrSet a, d, ''
call ArrSet a, f, ''
call ArrSet a, g, ''
call ArrSet a, h, ''

if \ArrIn(a, b) then fail(test, b 'not found')
if \ArrIn(a, c) then fail(test, c 'not found')
if \ArrIn(a, d) then fail(test, d 'not found')
if ArrIn(a, e) then fail(test, e 'found')
if \ArrIn(a, f) then fail(test, f 'not found')
if \ArrIn(a, g) then fail(test, g 'not found')
if \ArrIn(a, h) then fail(test, h 'not found')

if \ArrIn(a, b, c) then fail(test, b c)
if \ArrIn(a, b, d) then fail(test, b d)
if ArrIn(a, b, e) then fail(test, b e)
if \ArrIn(a, b, f) then fail(test, b f)
if \ArrIn(a, b, g) then fail(test, b g)
if \ArrIn(a, b, h) then fail(test, b h)

if ArrIn(a, b, c, d, e, f, g, h) then fail(test, 'all, order 1')
if ArrIn(a, b, e, c, d, f, g, h) then fail(test, 'all, order 2')
if ArrIn(a, b, c, e, d, f, g, h) then fail(test, 'all, order 3')
if ArrIn(a, b, c, d, f, e, g, h) then fail(test, 'all, order 4')
if ArrIn(a, b, c, d, f, g, e, h) then fail(test, 'all, order 5')
if ArrIn(a, b, c, d, f, g, h, e) then fail(test, 'all, order 6')
if \ArrIn(a, b, c, d, f, g, h) then fail(test, 'nearly all')

say 'pass' test

test = 'Drop 1'

call ArrDrop a, f

if \ArrIn(a, b) then fail(test, b 'not found')
if \ArrIn(a, c) then fail(test, c 'not found')
if \ArrIn(a, d) then fail(test, d 'not found')
if ArrIn(a, e) then fail(test, e 'found')
if ArrIn(a, f) then fail(test, f 'found')
if \ArrIn(a, g) then fail(test, g 'not found')
if \ArrIn(a, h) then fail(test, h 'not found')

say 'pass' test

test = 'Drop 2'

call ArrDrop a, d, h

if \ArrIn(a, b) then fail(test, b 'not found')
if \ArrIn(a, c) then fail(test, c 'not found')
if ArrIn(a, d) then fail(test, d 'found')
if ArrIn(a, e) then fail(test, e 'found')
if ArrIn(a, f) then fail(test, f 'found')
if \ArrIn(a, g) then fail(test, g 'not found')
if ArrIn(a, h) then fail(test, h 'found')

test = 'assign'

arr = a

if \ArrIn(arr, b) then fail(test, b 'not found')
if \ArrIn(arr, c) then fail(test, c 'not found')
if ArrIn(arr, d) then fail(test, d 'found')
if ArrIn(arr, e) then fail(test, e 'found')
if ArrIn(arr, f) then fail(test, f 'found')
if \ArrIn(arr, g) then fail(test, g 'not found')
if ArrIn(arr, h) then fail(test, h 'found')

call ArrDrop a, c

if \ArrIn(arr, b) then fail(test, b 'not found')
if ArrIn(arr, c) then fail(test, c 'found')
if ArrIn(arr, d) then fail(test, d 'found')
if ArrIn(arr, e) then fail(test, e 'found')
if ArrIn(arr, f) then fail(test, f 'found')
if \ArrIn(arr, g) then fail(test, g 'not found')
if ArrIn(arr, h) then fail(test, h 'found')

say 'pass' test

test = 'copy'

arr = ArrCopy(a)

if \ArrIn(arr, b) then fail(test, b 'not found')
if ArrIn(arr, c) then fail(test, c 'found')
if ArrIn(arr, d) then fail(test, d 'found')
if ArrIn(arr, e) then fail(test, e 'found')
if ArrIn(arr, f) then fail(test, f 'found')
if \ArrIn(arr, g) then fail(test, g 'not found')
if ArrIn(arr, h) then fail(test, h 'found')

call ArrDrop a, g

if \ArrIn(arr, b) then fail(test, b 'not found')
if ArrIn(arr, c) then fail(test, c 'found')
if ArrIn(arr, d) then fail(test, d 'found')
if ArrIn(arr, e) then fail(test, e 'found')
if ArrIn(arr, f) then fail(test, f 'found')
if \ArrIn(arr, g) then fail(test, g 'not found')
if ArrIn(arr, h) then fail(test, h 'found')

call ArrDrop a
call ArrDrop arr

say 'pass' test

test = 'iteration'

a = ArrNew()
s.b = 'Bob'
s.c = 'Charlie'
s.d = 'Doug'
s.f = 'Frank'
s.g = 'George'
s.h = 'Harry'

call ArrSet a, b, s.b
call ArrSet a, c, s.c
call ArrSet a, d, s.d
call ArrSet a, f, s.f
call ArrSet a, g, s.g
call ArrSet a, h, s.h

t. = 0

k = ArrDoOver(a, 1)
do while k \= ''
  if s.k \= ArrGet(a, k) then fail(test, 'a['k']' ArrGet(a,k) 's.'k s.k)
  t.k = 1
  k = ArrDoOver(a)
  end

if \t.b then fail(test, b 'not returned')
if \t.c then fail(test, c 'not returned')
if \t.d then fail(test, d 'not returned')
if \t.f then fail(test, f 'not returned')
if \t.g then fail(test, g 'not returned')
if \t.h then fail(test, h 'not returned')

call ArrDrop a

say 'pass' test

test = 'iteration with default'

a = ArrNew('Roderick')

call ArrSet a, b, s.b
call ArrSet a, c, s.c
call ArrSet a, d, s.d
call ArrSet a, f, s.f
call ArrSet a, g, s.g
call ArrSet a, h, s.h

t. = 0

k = ArrDoOver(a, 1)
do while k \= ''
  if s.k \= ArrGet(a, k) then fail(test, 'a['k']' ArrGet(a,k) 's.'k s.k)
  t.k = 1
  k = ArrDoOver(a)
  end

if \t.b then fail(test, b 'not returned')
if \t.c then fail(test, c 'not returned')
if \t.d then fail(test, d 'not returned')
if \t.f then fail(test, f 'not returned')
if \t.g then fail(test, g 'not returned')
if \t.h then fail(test, h 'not returned')

call ArrDrop a

say 'pass' test

test = 'nested iteration'

a = ArrNew()

call ArrSet a, b, s.b
call ArrSet a, c, s.c
call ArrSet a, d, s.d
call ArrSet a, f, s.f
call ArrSet a, g, s.g
call ArrSet a, h, s.h

arr = ArrCopy(a)

t. = 0

k = ArrDoOver(a, 1)
do while k \= ''
  if s.k \= ArrGet(a, k) then fail(test, 'a['k']' ArrGet(a,k) 's.'k s.k)
  t.k = 1
  tt. = 0
  kk = ArrDoOver(arr, 1)
  do while kk \= ''
    if s.kk \= ArrGet(arr, kk) then fail(test, 'arr['kk']' ArrGet(arr,kk) 's.'kk s.kk)
    tt.kk = 1
    kk = ArrDoOver(arr)
    end

  if \tt.b then fail(test, 'inner' b 'not returned')
  if \tt.c then fail(test, 'inner' c 'not returned')
  if \tt.d then fail(test, 'inner' d 'not returned')
  if \tt.f then fail(test, 'inner' f 'not returned')
  if \tt.g then fail(test, 'inner' g 'not returned')
  if \tt.h then fail(test, 'inner' h 'not returned')
  k = ArrDoOver(a)
  end

if \t.b then fail(test, 'outer' b 'not returned')
if \t.c then fail(test, 'outer' c 'not returned')
if \t.d then fail(test, 'outer' d 'not returned')
if \t.f then fail(test, 'outer' f 'not returned')
if \t.g then fail(test, 'outer' g 'not returned')
if \t.h then fail(test, 'outer' h 'not returned')

call ArrDrop a
call ArrDrop arr

say 'pass' test

test = 'from stem'

a = ArrFromStem('s.')
if \ArrIn(a, b) then fail(test, b 'missing')
if \ArrIn(a, c) then fail(test, c 'missing')
if \ArrIn(a, d) then fail(test, d 'missing')
if \ArrIn(a, f) then fail(test, f 'missing')
if \ArrIn(a, g) then fail(test, g 'missing')
if \ArrIn(a, h) then fail(test, h 'missing')

if ArrGet(a, b) \= s.b then fail(test, b ArrGet(a, b))
if ArrGet(a, c) \= s.c then fail(test, c ArrGet(a, c))
if ArrGet(a, d) \= s.d then fail(test, d ArrGet(a, d))
if ArrGet(a, e) \= '' then fail(test, e ArrGet(a, e))
if ArrGet(a, f) \= s.f then fail(test, f ArrGet(a, f))
if ArrGet(a, g) \= s.g then fail(test, g ArrGet(a, g))
if ArrGet(a, h) \= s.h then fail(test, h ArrGet(a, h))

call ArrDrop a

say 'pass' test

test = 'from stem with default'

s. = 'Edmund'
s.b = 'Bob'
s.c = 'Charlie'
s.d = 'Doug'
s.f = 'Frank'
s.g = 'George'
s.h = 'Harry'

a = ArrFromStem('s.')
if \ArrIn(a, b) then fail(test, b 'missing')
if \ArrIn(a, c) then fail(test, c 'missing')
if \ArrIn(a, d) then fail(test, d 'missing')
if \ArrIn(a, f) then fail(test, f 'missing')
if \ArrIn(a, g) then fail(test, g 'missing')
if \ArrIn(a, h) then fail(test, h 'missing')

if ArrGet(a, b) \= s.b then fail(test, b ArrGet(a, b))
if ArrGet(a, c) \= s.c then fail(test, c ArrGet(a, c))
if ArrGet(a, d) \= s.d then fail(test, d ArrGet(a, d))
if ArrGet(a, e) \= s. then fail(test, e ArrGet(a, e))
if ArrGet(a, f) \= s.f then fail(test, f ArrGet(a, f))
if ArrGet(a, g) \= s.g then fail(test, g ArrGet(a, g))
if ArrGet(a, h) \= s.h then fail(test, h ArrGet(a, h))

call ArrDrop a

say 'pass' test


test = 'to stem'

drop s.
s.b = 'Bob'
s.c = 'Charlie'
s.d = 'Doug'
s.f = 'Frank'
s.g = 'George'
s.h = 'Harry'

a = ArrFromStem('s.')
call ArrToStem a, 't.'

if ArrGet(a, b) \= t.b then fail(test, b t.b)
if ArrGet(a, c) \= t.c then fail(test, c t.c)
if ArrGet(a, d) \= t.d then fail(test, d t.d)
if 'T.E' \= t.e then fail(test, e t.e)
if ArrGet(a, f) \= t.f then fail(test, f t.f)
if ArrGet(a, g) \= t.g then fail(test, g t.g)
if ArrGet(a, h) \= t.h then fail(test, h t.h)

call ArrDrop a
drop t.

say 'pass' test

test = 'to stem with default'

s. = 'Edmund'
s.b = 'Bob'
s.c = 'Charlie'
s.d = 'Doug'
s.f = 'Frank'
s.g = 'George'
s.h = 'Harry'

a = ArrFromStem('s.')
call ArrToStem a, 't.'

if ArrGet(a, b) \= t.b then fail(test, b t.b)
if ArrGet(a, c) \= t.c then fail(test, c t.c)
if ArrGet(a, d) \= t.d then fail(test, d t.d)
if s. \= t.e then fail(test, e t.e)
if ArrGet(a, f) \= t.f then fail(test, f t.f)
if ArrGet(a, g) \= t.g then fail(test, g t.g)
if ArrGet(a, h) \= t.h then fail(test, h t.h)

call ArrDrop a
drop t.

say 'pass' test

test = 'procedure call'
a = ArrFromStem('s.')
call proctest a, test
if ArrIn(a, f) then fail(test, f 'found')
call ArrDrop a
say 'pass' test


say '**** All tests passed'

call ArrDropFuncs

exit 0

proctest: procedure
  parse arg a, test
  if \ArrIn(a, b, c, d, f, g, h) then fail(test)
  call ArrDrop a, f
  return

fail:
	parse arg reason
	say 'fail' reason 'at line' sigl
	call ArrDropFuncs
	exit 1
