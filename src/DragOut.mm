//
//  DragOUt.m
//  ViZa
//
//  Created by Martin Hermant on 06/09/2015.
//
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "DragOut.h"
//#import "ofAppRunner.h"
//==============================================================================
bool DragOut::performExternalDragDropOfFiles (const std::vector<std::string>& files, void * window,float x, float y)
{
    
    NSLog(@"erzfz");
    if (files.size() == 0)
        return false;
    
    //    MouseInputSource* draggingSource = Desktop::getInstance().getDraggingMouseSource(0);
    
    //    if (draggingSource == nullptr)
    //    {
    //        jassertfalse;  // This method must be called in response to a component's mouseDown or mouseDrag event!
    //        return false;
    //    }
    
    //    Component* sourceComp = draggingSource->getComponentUnderMouse();
    //
    //    if (sourceComp == nullptr)
    //    {
    //        jassertfalse;  // This method must be called in response to a component's mouseDown or mouseDrag event!
    //        return false;
    //    }
    
    //    JUCE_AUTORELEASEPOOL
    @autoreleasepool
    {
        
        // Write data to the pasteboard
        //        NSURL *fileURL; // Assume this exists
        //        NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
        //        [pboard declareTypes:[NSArray arrayWithObject:NSURLPboardType] owner:nil];
        //        [fileURL writeToPasteboard:pboard];
        NSWindow * mainWindow = (NSWindow*)   window;
        if (NSView* view = [mainWindow contentView] )
        {
            //            if (NSEvent* event = [[view window] currentEvent])

            if(NSEvent* event = [mainWindow currentEvent]){

//            if(NSEvent* event = [[[NSApplication sharedApplication] mainWindow] currentEvent]){
                {
                    NSPoint eventPos;
                    eventPos.x = (CGFloat)x;
                    eventPos.y = (CGFloat)y;// = [event locationInWindow];
                    NSRect dragRect = [view convertRect: NSMakeRect (eventPos.x - 16.0f, eventPos.y - 16.0f, 32.0f, 32.0f)
                                               fromView: view];
                    
                    for (int i = 0; i < files.size(); ++i)
                    {
                        if (! [view dragFile: [NSString stringWithCString:files[i].c_str()
                                                                 encoding:[NSString defaultCStringEncoding]]
                                    fromRect: dragRect
                                   slideBack: YES
                                       event: event])
                            NSLog(@"error drag out");
                            return false;
                        //                }
                        //
                        //                return true;
                        //            }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}