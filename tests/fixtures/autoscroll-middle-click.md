# Middle-click autoscroll

Press the **middle button** on this page and let it go: a round mark stays
where it was pressed. Move the pointer *below* the mark and the page runs
down; move it *above* and it runs up. The farther from the mark, the
faster. Within the mark nothing moves. Click any button, turn the wheel or
press a key to stop. Esc only stops; another key, such as **Home**, stops
and does what it does.

Hold the middle button instead and drag off the mark: the page runs while
it is held and stops where the button is let go.

> Autoscroll works on the page, on the editor's text and on the preview
> beside it, on the Contents (**Tab**), the file browser and the search
> results, and in Help (**F1**). The tab strip's middle click still closes
> a tab, and a sheet still pans with the middle button held.

The wide table and the long code line below give the page a sideways
scroll at narrow widths: there, moving left or right of the mark runs the
page across too, and the mark shows four ways instead of two.

| Column 1 | Column 2 | Column 3 | Column 4 | Column 5 | Column 6 | Column 7 | Column 8 | Column 9 | Column 10 | Column 11 | Column 12 | Column 13 | Column 14 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| alpha | beta | gamma | delta | epsilon | zeta | eta | theta | iota | kappa | lambda | mu | nu | xi |
| one | two | three | four | five | six | seven | eight | nine | ten | eleven | twelve | thirteen | fourteen |

```python
def autoscroll_speed(offset_dip, mark=14.0, factor=0.0032, power=1.3, cap=8.0):  # DIP per ms, a long line that runs past the right edge of a narrow window
    past = abs(offset_dip) - mark
    return 0.0 if past <= 0 else min(factor * past ** power, cap) * (1 if offset_dip > 0 else -1)
```

Jump to [the last section](#the-end) and back with **Home**.

## Section 1

Paragraph 1 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 1
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 2

Paragraph 2 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 2
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 3

Paragraph 3 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 3
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 4

Paragraph 4 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 4
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 5

Paragraph 5 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 5
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 5: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 6

Paragraph 6 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 6
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 7

Paragraph 7 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 7
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 8

Paragraph 8 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 8
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 9

Paragraph 9 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 9
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 10

Paragraph 10 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 10
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 10: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 11

Paragraph 11 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 11
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 12

Paragraph 12 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 12
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 13

Paragraph 13 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 13
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 14

Paragraph 14 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 14
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 15

Paragraph 15 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 15
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 15: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 16

Paragraph 16 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 16
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 17

Paragraph 17 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 17
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 18

Paragraph 18 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 18
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 19

Paragraph 19 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 19
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 20

Paragraph 20 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 20
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 20: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 21

Paragraph 21 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 21
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 22

Paragraph 22 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 22
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 23

Paragraph 23 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 23
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 24

Paragraph 24 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 24
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 25

Paragraph 25 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 25
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 25: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 26

Paragraph 26 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 26
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 27

Paragraph 27 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 27
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 28

Paragraph 28 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 28
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 29

Paragraph 29 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 29
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 30

Paragraph 30 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 30
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 30: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 31

Paragraph 31 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 31
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 32

Paragraph 32 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 32
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 33

Paragraph 33 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 33
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 34

Paragraph 34 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 34
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 35

Paragraph 35 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 35
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 35: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## Section 36

Paragraph 36 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 36
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 37

Paragraph 37 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 37
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 38

Paragraph 38 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 38
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 39

Paragraph 39 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 39
- Second point, with a nested list:
  1. one
  2. two
- Third point

## Section 40

Paragraph 40 runs long enough to wrap at a narrow width, with *emphasis*,
**bold text**, `inline code` and a [link to the top](#middle-click-autoscroll).
Autoscroll passes over it at the speed the pointer sets.

- First point of section 40
- Second point, with a nested list:
  1. one
  2. two
- Third point

> A quote in section 40: the mark is drawn above the page, the
> headings, the lists and the quotes as they pass under it.

## The end

The last section. Autoscroll held at the end of the page waits there for
the pointer to turn back above the mark.
