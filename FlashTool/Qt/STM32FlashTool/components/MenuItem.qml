import './forms'

MenuItemForm {
    id:menu_item
    property bool contains_mouse: false
    mouse_area.onContainsMouseChanged: () => {
                                  if(!contains_mouse)
                                    item_frame.opacity = 0.5
                                  else
                                  item_frame.opacity = 1.0
                                 contains_mouse = !contains_mouse
                              }
}
