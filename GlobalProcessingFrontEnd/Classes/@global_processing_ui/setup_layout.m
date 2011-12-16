function setup_layout(obj)

    handles = guihandles(obj.window);

    % Start layout
    %---------------------------------------
    main_layout = uiextras.VBoxFlex( 'Parent', obj.window, 'Spacing', 3 );
    top_layout = uiextras.HBoxFlex( 'Parent', main_layout, 'Spacing', 3 );
    left_layout = uiextras.VBoxFlex( 'Parent', top_layout, 'Spacing', 3 );
    

    % Decay Display
    %---------------------------------------
    
    right_layout = uiextras.VBoxFlex( 'Parent', top_layout );
    
    topright_layout = uiextras.HBox( 'Parent', right_layout );
            
    display_panel = uiextras.TabPanel( 'Parent', topright_layout );
    
    handles = obj.add_decay_display_panel(handles,display_panel);
    handles = obj.add_table_display_panel(handles,display_panel);
    handles = obj.add_image_display_panel(handles,display_panel);
    handles = obj.add_hist_corr_display_panel(handles,display_panel);
    handles = obj.add_plotter_display_panel(handles,display_panel);

    set(display_panel, 'TabNames', {'Decay','Parameters','Images','Gallery','Histogram','Correlation','Plotter','Plate'});
    set(display_panel, 'SelectedChild', 1);
    
    col_names = {'Plot','Display','Merge','Min','Max','Auto'};
    col_width = {60 30 30 50 50 30};
    handles.plot_select_table = uitable( 'ColumnName', col_names, 'ColumnWidth', col_width, 'RowName', [], 'Parent', topright_layout );

    set( topright_layout, 'Sizes', [-1, 0] );
   
    function display_panel_callback(~,src,~)
        if src.SelectedChild <= 2
            set( topright_layout, 'Sizes', [-1, 0] )
        else
            set( topright_layout, 'Sizes', [-1, 253] )
        end
    end
    
    set( display_panel, 'Callback', @display_panel_callback );
    
    % Progress Panel
    %---------------------------------------
    progress_panel = uiextras.BoxPanel( 'Parent', right_layout, 'Title', 'Progress' );
    handles.progress_table = uitable( 'Parent', progress_panel );
    
    set( right_layout, 'Sizes', [-1,110] );
    
    % Dataset Panel
    %---------------------------------------
        
    dataset_panel = uiextras.BoxPanel( 'Parent', left_layout, 'Title', 'Dataset' );
    dataset_layout = uiextras.HBox( 'Parent', dataset_panel, 'Padding', 3 );
    %handles.data_series_listbox = uicontrol( 'Style', 'listbox', 'Parent', dataset_layout );
    handles.data_series_table = uitable( 'Parent', dataset_layout );
    
    handles.intensity_container = uicontainer( 'Parent', dataset_layout ); 
    handles.intensity_axes = axes( 'Parent', handles.intensity_container );
    
    set( dataset_layout, 'Sizes', [150,-1] );

    
    % Data Transformation Panel
    %---------------------------------------
    handles = obj.add_data_transformation_panel(handles,left_layout);
 

    
    %set(main_layout,'Sizes',[-4,-1]);
    
    % Fitting Params Panel
    %---------------------------------------
    handles = obj.add_fitting_params_panel(handles, left_layout);

    fit_button_layout = uiextras.HBox( 'Parent', left_layout, 'Spacing', 3 );
    
    handles.binned_fit_pushbutton = uicontrol( 'Style', 'pushbutton', 'String', 'Binned Fit', 'Parent', fit_button_layout );
    handles.fit_pushbutton = uicontrol( 'Style', 'pushbutton', 'String', 'Fit', 'Parent', fit_button_layout );

    set(fit_button_layout,'Sizes',[-1,-2]);
    
    set(left_layout,'Sizes',[-1,140,280,30])
    
        
    set(top_layout,'Sizes',[550,-1]);

    dragzoom([handles.highlight_axes handles.residuals_axes])
   
    guidata(obj.window, handles);
    
end