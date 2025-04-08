patches-own [
  cooperate?       ;; patch will cooperate
  old-cooperate?   ;; patch has cooperated before
  score            ;; score resulting from interaction of neighboring patches
  color-class      ;; numeric value from 1= blue, 2= red, 3= green, 4= yellow.
]

to setup
  clear-all
  ask patches [
    ifelse random-float 1.0 < (initial-cooperation / 100)
      [setup-cooperation true]
      [setup-cooperation false]
    establish-color
  ]
  reset-ticks
  update-plot
end

to setup-cooperation [value]
  set cooperate? value
  set old-cooperate? value
end

to go
  ask patches [interact]          ;; to play with a neighboring patch
  ask patches [select-strategy]   ;; adopt the strategy of the neighbor (who had the highest score)
  tick
  update-plot
end

to update-plot
  set-current-plot "Cooperation/Defection Frequency"
  plot-histogram-helper "cc" blue
  plot-histogram-helper "dd" red
  plot-histogram-helper "cd" green
  plot-histogram-helper "dc" yellow
end

to plot-histogram-helper [pen-name color-name]
  set-current-plot-pen pen-name
  histogram [color-class] of patches with [pcolor = color-name]
end

to interact  ;; patch procedure
  let total-cooperaters count neighbors with [cooperate?]  ;; total number neighbors who cooperated
  ifelse cooperate?
    [set score total-cooperaters]                   ;; cooperator gets score of # of neighbors who cooperated
    [set score defection-award * total-cooperaters] ;; non-cooperator get score of a multiple of the
                                                    ;; neighbors who cooperated
end

to select-strategy  ;; patch procedure
  set old-cooperate? cooperate?
  set cooperate? [cooperate?] of max-one-of neighbors [score] ;;choose strategy (cooperate, not cooperate)
                                                            ;; of neighbor who performed the best
  establish-color
end

to establish-color  ;; patch procedure
  ifelse old-cooperate?
    [ifelse cooperate?
      [set pcolor blue
       set color-class 1]
      [set pcolor green
       set color-class 3]
    ]
    [ifelse cooperate?
      [set pcolor yellow
       set color-class 4]
      [set pcolor red
       set color-class 2]
    ]
end


; Copyright 2002 Uri Wilensky.
; See Info tab for full copyright and license.