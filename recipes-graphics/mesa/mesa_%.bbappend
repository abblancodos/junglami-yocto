# Panfrost para Mali-G52 (RK3566), Wayland-only
PACKAGECONFIG:append = " opengl panfrost kmsro gbm egl gles"
PACKAGECONFIG:remove = "x11 dri3"
