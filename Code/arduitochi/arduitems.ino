void using_item_draw(){
  switch (usingobject[1]){
    case 0: //Plant
    display.setCursor(22,12);
    display.print("Plants!");
      ardudraw(38, 32, 0);
      display.drawBitmap(50, 32, item_00_icon_bmp, 12, 12, 1);
      display.drawBitmap(24, 32, item_00_icon_bmp, 12, 12, 1);
    break;
    default: //If I dont have idea of this item
    display.setCursor(22,28);
    display.print("Soon");
    break;
  }
}

