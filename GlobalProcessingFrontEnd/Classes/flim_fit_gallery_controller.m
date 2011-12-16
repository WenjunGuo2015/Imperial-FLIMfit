classdef flim_fit_gallery_controller < abstract_plot_controller

    properties
        gallery_cols_edit;
        gallery_overlay_popupmenu;
        gallery_unit_edit; 
    end
    
    methods
        function obj = flim_fit_gallery_controller(handles)
                       
            obj = obj@abstract_plot_controller(handles,handles.gallery_panel,handles.gallery_param_popupmenu);            
            assign_handles(obj,handles);
            
             addlistener(obj.plot_handle,'Position','PostSet',@(~,~,~) obj.update_display);

            set(obj.gallery_cols_edit,'Callback',@obj.gallery_params_update);
            set(obj.gallery_overlay_popupmenu,'Callback',@obj.gallery_params_update);
            set(obj.gallery_unit_edit,'Callback',@obj.gallery_params_update);
            
            obj.update_display();
        end
        
        function gallery_params_update(obj,~,~)
            cols = round(str2double(get(obj.gallery_cols_edit,'String')));
            set(obj.gallery_cols_edit,'String',num2str(cols));
            
            obj.update_display();
        end
        
        function plot_fit_update(obj)
            if obj.fit_controller.has_fit
                r = obj.fit_controller.fit_result;
                metafields = fieldnames(r.metadata);
                set(obj.gallery_overlay_popupmenu,'String',['-' metafields']);
            end
        end
        
        function draw_plot(obj,f,param)
          
            save = (f ~= obj.plot_handle);
            
            overlay = get(obj.gallery_overlay_popupmenu,'Value');
            if overlay == 1
                overlay = [];
            else
                names = get(obj.gallery_overlay_popupmenu,'String');
                overlay = names{overlay};
            end

            unit = get(obj.gallery_unit_edit,'String');
            cols = str2double(get(obj.gallery_cols_edit,'String'));

            d = obj.fit_controller.data_series;
            n_im = d.n_datasets;

            rows = ceil(n_im/cols);

            if n_im>0
                [ha,hc] = tight_subplot(f,n_im,rows,cols,save,[d.width d.height],5,5);
            end

            if ~strcmp(param,'-')

                if isempty(overlay);
                    meta = [];
                else
                    meta = d.metadata.(overlay);
                end

                for i=1:n_im
                    if ~isempty(meta)
                        text = meta{i};
                        if strcmp(overlay,'s') && strcmp(unit,'min')
                            text = text / 60;
                        end
                        if ~ischar(text)
                            text = num2str(text);
                        end
                        text = [text ' ' unit];
                    else
                        text = '';
                    end

                    obj.plot_figure(ha(i),hc(i),i,param,false,text);
                end

            end
            
        end
        
    end
end