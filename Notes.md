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





