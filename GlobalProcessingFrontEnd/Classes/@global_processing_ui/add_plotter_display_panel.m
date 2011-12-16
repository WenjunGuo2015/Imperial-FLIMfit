function handles = add_plotter_display_panel(obj,handles,parent)

    layout = uiextras.VBox( 'Parent', parent, 'Spacing', 3 );
    
    handles.graph_axes = axes('Parent',layout);
    
    param_layout = uiextras.Grid( 'Parent', layout, 'Spacing', 3 );
    uicontrol( 'Style', 'text', 'String', 'Label  ', 'Parent', param_layout, ...
               'HorizontalAlignment', 'right' );
    uicontrol( 'Style', 'text', 'String', 'Parameter  ', 'Parent', param_layout, ...
               'HorizontalAlignment', 'right' );
           
    handles.graph_independent_popupmenu = uicontrol( 'Style', 'popupmenu', ...
            'String', {''}, 'Parent', param_layout );
    handles.graph_dependent_popupmenu = uicontrol( 'Style', 'popupmenu', ...
            'String', {''}, 'Parent', param_layout );
        
    set( param_layout, 'RowSizes', [22,22] );
    set( param_layout, 'ColumnSizes', [100,200] );
    
    set( layout, 'Sizes', [-1 70])
    
    
    plate_layout = uiextras.VBox( 'Parent', parent, 'Spacing', 3 );
    
        
    plate_container = uicontainer( 'Parent', plate_layout );
    handles.plate_axes = axes( 'Parent', plate_container );
    
    param_layout = uiextras.Grid( 'Parent', plate_layout, 'Spacing', 3 );
    uicontrol( 'Style', 'text', 'String', 'Parameter  ', 'Parent', param_layout, ...
               'HorizontalAlignment', 'right' );
    handles.plate_param_popupmenu = uicontrol( 'Style', 'popupmenu', ...
            'String', {''}, 'Parent', param_layout );
    
    set( param_layout, 'RowSizes', [22] );
    set( param_layout, 'ColumnSizes', [100,200] );
    
    set( plate_layout, 'Sizes', [-1 70])
    
    
    
    

end