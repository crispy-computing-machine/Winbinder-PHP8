<?php

class CollapsibleSettingsUI
{
    private $window;
    private $nextY = 16;
    private $panelIds = [];

    public function __construct($title, $x, $y, $width, $height)
    {
        $this->window = wb_create_window(0, AppWindow, $title, $x, $y, $width, $height, WBC_NOTIFY, WBC_RESIZE);
    }

    public function getWindow()
    {
        return $this->window;
    }

    public function getPanelIds()
    {
        return $this->panelIds;
    }

    public function addPanel($id, $title, $icon = null)
    {
        $panel = wb_create_control($this->window, Frame, $title, 14, $this->nextY, 490, 44, $id, WBC_PANEL);
        wb_panel_set_header($panel, $title, $icon);

        $this->panelIds[] = $id;
        return new CollapsiblePanelBuilder($panel, $id, $this);
    }

    public function finalizePanel($panel, $contentRows)
    {
        $height = 44 + ($contentRows * 26);
        wb_set_size($panel, 490, $height);
        $this->nextY += $height + 8;
    }

    public function resizePanelsToWindow()
    {
        $windowSize = wb_get_size($this->window);
        foreach ($this->panelIds as $id) {
            $panel = wb_get_control($this->window, $id);
            if ($panel) {
                $panelSize = wb_get_size($panel);
                wb_set_size($panel, $windowSize[0] - 30, $panelSize[1]);
            }
        }
    }
}

class CollapsiblePanelBuilder
{
    private $panel;
    private $rowCount = 0;
    private $owner;

    public function __construct($panel, $id, CollapsibleSettingsUI $owner)
    {
        $this->panel = $panel;
        $this->owner = $owner;
    }

    public function addLabel($text)
    {
        $y = 28 + ($this->rowCount * 26);
        wb_create_control($this->panel, Label, $text, 16, $y + 4, 220, 16, 0);
        $this->rowCount++;
        return $this;
    }

    public function addEditBox($text)
    {
        $y = 28 + ($this->rowCount * 26);
        wb_create_control($this->panel, EditBox, $text, 220, $y, 230, 22, 0);
        $this->rowCount++;
        return $this;
    }

    public function addCheckBox($text)
    {
        $y = 28 + ($this->rowCount * 26);
        wb_create_control($this->panel, CheckBox, $text, 16, $y, 434, 22, 0);
        $this->rowCount++;
        return $this;
    }

    public function done()
    {
        $this->owner->finalizePanel($this->panel, $this->rowCount);
        return $this->owner;
    }
}

$settingsUI = new CollapsibleSettingsUI('Settings', 80, 80, 520, 500);

$settingsUI->addPanel(101, 'General')
    ->addLabel('Language')
    ->addEditBox('English (US)')
    ->addCheckBox('Check for updates automatically')
    ->done();

$settingsUI->addPanel(102, 'Editor')
    ->addCheckBox('Use soft tabs')
    ->addLabel('Tab width')
    ->addEditBox('4')
    ->addCheckBox('Show line numbers')
    ->done();

$settingsUI->addPanel(103, 'Advanced')
    ->addCheckBox('Enable experimental features')
    ->addCheckBox('Verbose logs')
    ->addEditBox('Custom arguments...')
    ->done();

wb_set_handler($settingsUI->getWindow(), 'process_main');
wb_main_loop();

function process_main($window, $id, $ctrl, $param1, $param2)
{
    global $settingsUI;

    if ($id === IDCLOSE) {
        wb_destroy_window($window);
        return;
    }

    if ($param1 === WBC_PANEL_EXPANDED || $param1 === WBC_PANEL_COLLAPSED) {
        $state = ($param1 === WBC_PANEL_EXPANDED) ? 'expanded' : 'collapsed';
        wb_set_text($window, "Group #{$id} {$state}");
        return;
    }

    if ($param1 === WBC_RESIZE) {
        $settingsUI->resizePanelsToWindow();
    }
}
