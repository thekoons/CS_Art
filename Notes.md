    This is a quest for trippy visuals, especially to be paired with music.




Notes on experimenting with gifmaking:

    On rule switches, also switch how pixels are turned on and off
        So that not the same stuff is happening from rule to rule

    Different rules for R, B, G
        Overlapping patterns

    Green color still being misread upon image intake
        Gotta fix bit-masking system (gonna be a biiiitch)

        Natural images look strange because of it

        
    Symmetric/geometric images work the best

    For natural images, I'll need to:
        Fix green color issue
        Implement edge detection? Shape detection?
        Be conscious of which rules are used such that the image isn't taken over

    Need to investigate how to create rulesets that:
        aren't random
        "breathe"
        Give some sense of ebb and flow
        Alternate between expanding and condensing

        This could take the shape of a database that connects rules to lifetypes
            Then the algorithm could choose rules based on their behavior




    Done (Investigate compiler flags to accommodate new options)

    Compiler Options:

        Night Mode vs Day Mode
            'Night Mode' = dead pixel is inverted, live pixel is OG image
            'Day Mode' = Live pixel is inverted, dead pixel is OG image

        Random Rules vs Specified Rules

        Activation of Pixel by Pixel Color vs Edge/Shape Detection



Adding Scripted Animation and Filtering

    Now that the framework exists by which to generate a gif with n amount of frames, I can start implementing scripted events within the gifs. I propose doing this by creating the following class structure:

        class Event {
            public:
                Event(int start, int duration);

                virtual void Activate() = 0;

            private:
                int start;
                int duration;
        }

        class Animation : public Event {
            public:
                Animation();
                ~Animation();

                //Called once per frame, calls Write_Pixel appropriately
                void Activate();   

            private:
                int* rgb_start;     //Initial color values of animation
                int** rgb_script;   //Frame by frame delta color values
                                        //To be added to rgb values each frame

                int* pix_start;     //Initial coordinates of central pixel
                int** pix_script;   //Frame by frame delta coordinate values
                                        //To be added to coordinate each frame

                int shape;          //One of a predetermined number of shapes
                int* shape_params;  //Parameters unique to shape 

                //Writes a color value to a pixel
                void Write_Pixel(Bitmap& b, int* coords, int* rgb);
                void Apply_Filter(Bitmap& b, int* coords, int shape);
        }

        class Filter : public Event {
            public:
                Filter(int type);

                void Activate();

            private:
                int _type;

                void Apply_Filter();

                void pixellate();
                void grayscale();
                void blur();
                void cellshade();
                

        }

        





Integrate generation from music

    Use wav library function OpenRead() to read sample data from wav files

    Use wav library function GetNumSamples() to read number of samples

    Read samples one by one

    Identify n different "scopes" of pitches

    Identify effects to pair with those "scopes"


    So how does one ascertain the pitches present in a sample?

    What is a "sample"?

        

