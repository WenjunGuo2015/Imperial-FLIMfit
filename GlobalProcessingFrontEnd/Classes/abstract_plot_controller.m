classdef abstract_plot_controller < flim_fit_observer

    properties
        plot_handle;
        handle_is_axes;
        param_popupmenu;
        contextmenu;
        window;
        param_list;
        cur_param;

        data_series_list;
    end
    
    methods(Abstract = true)
        
        draw_plot(obj,ax,param);
    end
    
    methods
        
        function obj = abstract_plot_controller(handles,plot_handle,param_popupmenu)
                       
            obj = obj@flim_fit_observer(handles.fit_controller);
            obj.plot_handle = plot_handle;
            
            obj.handle_is_axes = strcmp(get(plot_handle,'type'),'axes');
            
            if nargin == 3
                obj.param_popupmenu = param_popupmenu;
                set(obj.param_popupmenu,'Callback',@obj.param_select_update);
            else
                obj.param_popupmenu = [];
            end
            
            assign_handles(obj,handles);

            obj.contextmenu = uicontextmenu('Parent',obj.window);
            uimenu(obj.contextmenu,'Label','Save as...','Callback',...
                @(~,~,~) obj.save_as() );
            uimenu(obj.contextmenu,'Label','Save as Powerpoint...','Callback',...
                @(~,~,~) obj.save_as_ppt() );
            uimenu(obj.contextmenu,'Label','Export to Current Powerpoint','Callback',...
                @(~,~,~) obj.export_to_ppt() );
            set(obj.plot_handle,'uicontextmenu',obj.contextmenu);
        end
        
        function save_as(obj)
            default_path = getpref('GlobalAnalysisFrontEnd','DefaultFolder');
            [filename, pathname, ~] = uiputfile( ...
                        {'*.tiff', 'TIFF image (*.tiff)';...
                         '*.pdf','PDF document (*.pdf)';...
                         '*.png','PNG image (*.png)';...
                         '*.eps','EPS level 1 image (*.eps)';...
                         '*.fig','Matlab figure (*.fig)';...
                         '*.*',  'All Files (*.*)'},...
                         'Select root file name',[default_path filesep]);

            if ~isempty(filename)
                
                [~,name,ext] = fileparts(filename);
                ext = ext(2:end);
                
                f = figure('Visible','off');
                if obj.handle_is_axes
                    ref = axes('Parent',f);
                else
                    ref = f;
                end
                
                obj.draw_plot(ref,obj.cur_param);
                
                savefig([pathname filesep name ' ' obj.cur_param],f,ext);
                close(f);
            end
            
        end
        
        function save_as_ppt(obj)
            if ispref('GlobalAnalysisFrontEnd','LastFigureExportFolder')
                default_path = getpref('GlobalAnalysisFrontEnd','LastFigureExportFolder');
            else
                default_path = getpref('GlobalAnalysisFrontEnd','DefaultFolder');
            end
            
            [filename, pathname, ~] = uiputfile( ...
                        {'*.ppt', 'Powerpoint (*.ppt)'},...
                         'Select root file name',[default_path filesep]);

            if ~isempty(filename)
                
                
                f = figure('Visible','off');
                if obj.handle_is_axes
                    ref = axes('Parent',f);
                else
                    ref = f;
                end
                             
                obj.draw_plot(ref,obj.cur_param);
                
                [~,name,ext] = fileparts(filename);
                file = [pathname filesep name ' ' obj.cur_param ext];
                if length(get(f,'children')) == 1 % if only one axis use pptfigure, gives better plots
                    ppt=saveppt2(file,'init');
                    pptfigure(f,'ppt',ppt);
                    saveppt2(file,'ppt',ppt,'close');
                else
                    saveppt2(file,'figure',f,'stretch',false);
                end
                setpref('GlobalAnalysisFrontEnd','LastFigureExportFolder',pathname);
            end
        end
        
        function export_to_ppt(obj)
            f = figure('Visible','on');
            if obj.handle_is_axes
                ref = axes('Parent',f);
            else
                ref = f;
            end
            
            obj.draw_plot(ref,obj.cur_param);
            if length(get(f,'children')) == 1 % if only one axis use pptfigure, gives better plots
                pptfigure(f,'SlideNumber','append');
            else
                saveppt2('current','figure',f,'stretch',false);
            end
            close(f);
        end
            
        function plot_fit_update(obj)
        end
        
        function update_param_menu(obj)
            if obj.fit_controller.has_fit
                obj.param_list = obj.fit_controller.fit_result.fit_param_list();
                
                if ~isempty(obj.param_popupmenu)    
                    set(obj.param_popupmenu,'String',obj.param_list);
                end
                
                if get(obj.param_popupmenu,'Value') > length(obj.param_list)
                    set(obj.param_popupmenu,'Value',1);
                end
            
                obj.param_select_update();    
            end
            
            
        end
        
        function param_select_update(obj,~,~)
            idx = get(obj.param_popupmenu,'Value');
            obj.cur_param = obj.param_list{idx};
            
            obj.update_display();
        end
        
        function fit_update(obj)
            obj.update_param_menu();
            obj.plot_fit_update();
            obj.update_display();
        end
        
        function update_display(obj)
            obj.draw_plot(obj.plot_handle,obj.cur_param);
        end
        
        function mapped_data = apply_colourmap(data,param,lims)
            
            cscale = obj.colourmap(param);
            
            m=2^16;
            data = data - lims(1);
            data = data / (lims(2) - lims(1));
            data(data > 1) = 1;
            data(data < 0) = 0;
            data = data * m + 1;
            data(isnan(data)) = 0;
            data = int32(data);
            cmap = cscale(m);
            cmap = [ [1,1,1]; cmap];
            
            mapped_data = ind2rgb(data,cmap);
            
        end
        
        function cscale = colourscale(obj,param)
            
            if strcmp(param,'I0')
                cscale = @hot;
            elseif ~isempty(strfind(param,'tau')) || ~isempty(strfind(param,'theta'))
                cscale = @inv_jet;
            else
                cscale = @jet;
            end
            
        end
        
        function im_data = plot_figure(obj,h,hc,dataset,im,merge,text)

            if ~obj.fit_controller.has_fit || (~isempty(obj.fit_controller.fit_result.binned) && obj.fit_controller.fit_result.binned == 1)
                return
            end

            d = obj.fit_controller.data_series;
            r = obj.fit_controller.fit_result;

            intensity = r.get_image(dataset,'I0');
            im_data = r.get_image(dataset,im);

            cscale = obj.colourscale(im);

            if ~merge
                im=colorbar_flush(h,hc,im_data,isnan(intensity),r.default_lims.(im),cscale,text);
            else
                im=colorbar_flush(h,hc,im_data,[],obj.plot_lims.(im),cscale,text,intensity,r.default_lims.I0);
            end
            

            if get(h,'Parent')==obj.plot_handle
                set(im,'uicontextmenu',obj.contextmenu);
            end
            
        end
       
    end
    
end