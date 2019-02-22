  
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
   //SETTING DESCRIPTION FOR A GEAR REDUCTION BASED MACHINE
   //Setting #0: Put a 0 here when using a machine that has a gear reduction system for both the altitude and azimuth movement.
   //Setting #1: Put 1 here to make the machine track the sun / Put 2 here to make the machine act as a heliostat / Put 3 here to make the machine act as a single axis sun tracker
   
   //SETTINGS FOR MACHINE'S ALTITUDE ROTATION 
   //Setting #2: Put the altitude gear ratio here
   //Setting #3:If you find that your altitude motor is spinning in the wrong direction, put a -1 here instead of a 1
   // [or the other way around if its already -1], Put a 0 here to keep this motor from moving.
   //Setting #4: Not used, put 0
   //Setting #5: Not used, put 0
   //Setting #6: Not used, put 0   
   //Setting #7: Not used, put 0   
   //Setting #8: Put the angle where the altitude limit switch is tripped here. Don't use 0
   //END OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 

   //SETTINGS FOR MACHINE'S AZIMUTH ROTATION      
   //Setting #9 Put the azimuth gear ratio here
   //Setting #10 If you find that your azimuth motor is spinning in the wrong direction, put a -1 here instead of a 1
   // [or the other way around if its already -1], Put a 0 here to keep this motor from moving.
   //Setting #11 Not used, put 0
   //Setting #12 Not used, put 0
   //Setting #13 Not used, put 0
   //Setting #14 Not used, put 0
   //Setting #15 Put the angle where the azimuth limit switch is tripped here
   //END OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION 

   //SETTINGS THAT LIMIT THE MACHINE'S MOTION
   //Setting #16 Put the minimum azimuth that your machine can move to here
   //Setting #17 Put the minimum altitude that your machine can move to here
   //Setting #18 Put the maximum azimuth that your machine can move to here 
   //Setting #19 Put the maximum altitude that your machine can move to here
   //END OF SETTINGS THAT LIMIT THE MACHINE'S MOTION

   //Setting #17 Put the maximum possible altitude for your machine here      
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        


   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //SETTING DESCRIPTION FOR A LINEAR ACTUATOR MACHINE
   //Setting #0: Put a 1 here when using a machine that has a linear actuator system for both the altitude and azimuth motion.
   //Setting #1: Put 1 here to make the machine track the sun / Put 2 here to make the machine act as a heliostat / Put 3 here to make the machine act as a single axis sun tracker
   
   //BEGINNING OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 
   //Setting #2: Put the altitude linear actuator's Turns Per Length here (Make sure to use the same measurement system for the length that you use for everything else)
   //Setting #3:If you find that your altitude motor is spinning in the wrong direction, put a -1 here instead of a 1
   // [or the other way around if its already -1], Put a 0 here to keep this motor from moving.  
   //Setting #4: Put Length A of the altitude linear actuator here
   //Setting #5: Put Length B of the altitude linear actuator here
   //Setting #6: Put the starting angle (when altitude = 0) of the altitude linear actuator here 
   //Setting #7: Put 1 here if the altitude linear actuator becomes more acute when it moves in the positive direction. If not, put 2  
   //Setting #8: Put the angle where the altitude limit switch is tripped here. Don't use 0
   //END OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 
   
   //BEGINNING OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION      
   //Setting #9 Put the azimuth linear actuator's Turns Per Length here (Make sure to use the same measurement system for the length that you use for everything else)
   //Setting #10 If you find that your azimuth motor is spinning in the wrong direction, put a -1 here instead of a 1
   //[or the other way around if its already -1], Put a 0 here to keep this motor from moving.
   //Setting #11 Put Length A of the azimuth linear actuator here
   //Setting #12 Put Length B of the azimuth linear actuator here
   //Setting #13 Put the starting angle (when azimuth = 0) of the azimuth linear actuator here 
   //Setting #14 Put 1 here if the azimuth linear actuator becomes more acute when it moves in the positive direction. If not, put 2
   //Setting #15 Put the angle where the azimuth limit switch is tripped here.
   //END OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION 

   //SETTINGS THAT LIMIT THE MACHINE'S MOTION
   //Setting #16 Put the minimum azimuth that your machine can move to here
   //Setting #17 Put the minimum altitude that your machine can move to here
   //Setting #18 Put the maximum azimuth that your machine can move to here 
   //Setting #19 Put the maximum altitude that your machine can move to here
   //END OF SETTINGS THAT LIMIT THE MACHINE'S MOTION

   //Setting #17 Put the maximum possible altitude for your machine here   
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

 

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
   //SETTING DESCRIPTION FOR LINEAR ACTUATOR ON ALTITUDE AND GEAR REDUCTION ON AZIMUTH MACHINE
   //Setting #0: Put a 2 here when using a machine that has a linear actuator system for the altitude motion and a gear reduction system for the azimuth motion.
   //Setting #1: Put 1 here to make the machine track the sun / Put 2 here to make the machine act as a heliostat / Put 3 here to make the machine act as a single axis sun tracker
   
   //BEGINNING OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 
   //Setting #2: Put the altitude linear actuator's Turns Per Length here (Make sure to use the same measurement system for the length that you use for everything else)
   //Setting #3:If you find that your altitude motor is spinning in the wrong direction, put a -1 here instead of a 1
   // [or the other way around if its already -1], Put a 0 here to keep this motor from moving.  
   //Setting #4: Put Length A of the altitude linear actuator here
   //Setting #5: Put Length B of the altitude linear actuator here
   //Setting #6: (Put the starting angle (when altitude = 0) of the altitude linear actuator here)    
   //Setting #7: (Put 1 here if the altitude linear actuator becomes more acute when it moves in the positive direction. If not, put 2)    
   //Setting #8: Put the angle where the altitude limit switch is tripped here. Don't use 0
   //END OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 

   //SETTINGS FOR MACHINE'S AZIMUTH ROTATION      
   //Setting #9 Put the azimuth gear ratio here
   //Setting #10 (If you find that your azimuth motor is spinning in the wrong direction, put a -1 here instead of a 1
   //[or the other way around if its already -1]), Put a 0 here to keep this motor from moving. 
   //Setting #11 Not used, put 0
   //Setting #12 Not used, put 0
   //Setting #13 Not used, put 0
   //Setting #14 Not used, put 0
   //Setting #15 Put the angle where the azimuth limit switch is tripped here.
   //END OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION 

   //SETTINGS THAT LIMIT THE MACHINE'S MOTION
   //Setting #16 Put the minimum azimuth that your machine can move to here
   //Setting #17 Put the minimum altitude that your machine can move to here
   //Setting #18 Put the maximum azimuth that your machine can move to here 
   //Setting #19 Put the maximum altitude that your machine can move to here
   //END OF SETTINGS THAT LIMIT THE MACHINE'S MOTION

   //Setting #17 Put the maximum possible altitude for your machine here      
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
   //SETTING DESCRIPTION FOR GEAR REDUCTION ON ALTITUDE AND LINEAR ACTUATOR ON AZIMUTH MACHINE
   //Setting #0: Put a 3 here when using a machine that has a gear reduction system for the altitude motion and a linear actuator system for the azimuth motion.
   //Setting #1: Put 1 here to make the machine track the sun / Put 2 here to make the machine act as a heliostat / Put 3 here to make the machine act as a single axis sun tracker 
   
   //SETTINGS FOR MACHINE'S ALTITUDE ROTATION 
   //Setting #2: Put the altitude gear ratio here
   //Setting #3:If you find that your altitude motor is spinning in the wrong direction, put a -1 here instead of a 1
   // [or the other way around if its already -1], Put a 0 here to keep this motor from moving. 
   //Setting #4: Not used, put 0
   //Setting #5: Not used, put 0
   //Setting #6: Not used, put 0   
   //Setting #7: Not used, put 0   
   //Setting #8: Put the angle where the altitude limit switch is tripped here. Don't use 0
   //END OF SETTINGS FOR MACHINE'S ALTITUDE ROTATION 

   //BEGINNING OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION      
   //Setting #9 Put the azimuth linear actuator's Turns Per Length here (Make sure to use the same measurement system for the length that you use for everything else)
   //Setting #10 If you find that your azimuth motor is spinning in the wrong direction, put a -1 here instead of a 1
   //[or the other way around if its already -1], Put a 0 here to keep this motor from moving.
   //Setting #11 Put Length A of the azimuth linear actuator here
   //Setting #12 Put Length B of the azimuth linear actuator here
   //Setting #13 Put the starting angle (when azimuth = 0) of the azimuth linear actuator here
   //Setting #14 Put 1 here if the azimuth linear actuator becomes more acute when it moves in the positive direction. If not, put 2
   //Setting #15 Put the angle where the azimuth limit switch is tripped here.
   //END OF SETTINGS FOR MACHINE'S AZIMUTH ROTATION 

   //SETTINGS THAT LIMIT THE MACHINE'S MOTION
   //Setting #16 Put the minimum azimuth that your machine can move to here
   //Setting #17 Put the minimum altitude that your machine can move to here
   //Setting #18 Put the maximum azimuth that your machine can move to here 
   //Setting #19 Put the maximum altitude that your machine can move to here
   //END OF SETTINGS THAT LIMIT THE MACHINE'S MOTION

   //Setting #17 Put the maximum possible altitude for your machine here      
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

