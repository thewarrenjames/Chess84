# ----------------------------
# Makefile Options
# ----------------------------

NAME = CHESS
ICON = media/icons/icon16.png
DESCRIPTION = "Chess84 v1.0.0 by Warren James"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
